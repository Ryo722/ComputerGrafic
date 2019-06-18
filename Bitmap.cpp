#include "Bitmap.h"
#include <cstring>   // memsetに必要
#include <fstream>   // ofstreamとifstreamに必要

MyBitmap::MyBitmap(int x, int y)
{
	if (x <= 0) x = 1;  // 不正なサイズだったら、とりあえず1ピクセルにしてしまう
	if (y <= 0) y = 1;
	std::memset(&this->bfh, 0x00, sizeof(MyBitmap::BITMAPFILEHEADER));  // 簡易初期化
	std::memset(&this->bih, 0x00, sizeof(MyBitmap::BITMAPINFOHEADER));
	
	// ビットマップファイルでは、横幅は4の倍数バイトと定義されるので、
	// 中途半端な横幅の場合、ゴミデータが末尾に置かれる
	// そこで、ゴミデータの量を調べる必要がある
	// ここでは、x*3とすることで、必要とされる横幅バイト数を調べた後、
	// 4で割った余りを4から引くことで、不足分のバイト数（ゴミデータ量）を調べる
	// けれども、このままだと余り0＝不足0のとき4になってしまう
	// ところで、0～4までの二進数表現は
	// 0→000
	// 1→001
	// 2→010
	// 3→011
	// 4→100
	// なので、下2桁だけ取り出せれば辻褄が合いそうだ
	// そこで、& 0x03（0x03は二進数で011と同じ）とやって、
	// 下二桁部分だけを取り出す
	const unsigned int padding = (4 - (x*3)%4) & 0x03;
	
	// ヘッダ情報を準備する
	// ここで書き込まなかったものは0でOK
	this->bfh.type = 0x4d42;  // BMPファイルですよーという意味
	this->bfh.size = 14 + 40 + (x*3 + padding)*y;  // 画像ファイルのサイズ
	this->bfh.offset = 14 + 40;  // さっきも出てきたけれども、これは画像のヘッダサイズ
	
	// このヘッダのサイズ……なのだけれども、
	// コンパイラによっては勝手に最適化してサイズがおかしくなるので、
	// 直接指定してあげている
	this->bih.header_size = 40;
	this->bih.width = x;
	this->bih.height = y;
	// 使用するプレーンとかいうものを定めるらしいが、常に1でOK
	// たぶん、0が一次元、1が二次元
	this->bih.planes = 1;
	this->bih.bits = 24;  // 24ビットマップですよーということ
	
	this->color = new Color[x*y];  // ピクセル数分の配列を作る
	
	// 白で初期化
	for (int i = 0; i < x; ++i) {
		for (int j = 0; j < y; ++j) {
			color[i + j*x].r = color[i + j*x].g = color[i + j*x].b = 0xff;
		}
	}
}

MyBitmap::MyBitmap(const char* const filename)
{
	std::memset(&this->bfh, 0x00, sizeof(MyBitmap::BITMAPFILEHEADER));
	std::memset(&this->bih, 0x00, sizeof(MyBitmap::BITMAPINFOHEADER));
	
	// ifstreamはInput File Streamのことで、
	// 入力（読み込み）用のストリームクラス
	std::ifstream ifs(filename, std::ios::binary);  // バイナリとして開く
	if (ifs.fail()) throw "ファイルが開けないよ";  // ファイルを開くのに失敗
	
	// ビットマップヘッダ情報が異常
	ifs.read((char*)&this->bfh.type, sizeof(unsigned short));
	if (this->bfh.type != 0x4d42) {
		ifs.close();
		throw "ビットマップファイルじゃないよ";
	}
	
	
	// ヘッダ読み込み
	// 細々と読み込んでるのは、コンパイラが構造体を最適化して
	// 妙なところにデータが入ると危ないので
	// ヘッダ情報の意味はとりあえず無視してOK
	
	// ファイル全体のサイズが入る
	ifs.read((char*)&this->bfh.size, sizeof(unsigned long));
	ifs.read((char*)&this->bfh.reserved1, sizeof(unsigned short));
	ifs.read((char*)&this->bfh.reserved2, sizeof(unsigned short));
	ifs.read((char*)&this->bfh.offset, sizeof(unsigned long));
	
	// まだまだヘッダ読み込み
	ifs.read((char*)&this->bih.header_size, sizeof(unsigned long));
	
	// 画像の縦横サイズ
	// ここだけは知っておくと良い
	ifs.read((char*)&this->bih.width, sizeof(long));
	ifs.read((char*)&this->bih.height, sizeof(long));
	ifs.read((char*)&this->bih.planes, sizeof(unsigned short));
	ifs.read((char*)&this->bih.bits, sizeof(unsigned short));
	ifs.read((char*)&this->bih.compression, sizeof(unsigned long));
	
	// 今回は使わないけれども、
	// ヘッダを除いたファイルサイズが書かれている
	ifs.read((char*)&this->bih.data_size, sizeof(unsigned long));
	ifs.read((char*)&this->bih.x_resolution, sizeof(long));
	ifs.read((char*)&this->bih.y_resolution, sizeof(long));
	ifs.read((char*)&this->bih.colours, sizeof(unsigned long));
	ifs.read((char*)&this->bih.important_colour, sizeof(long));
	
	// マイナーヘッダだったり、24ビットマップじゃなかったり、
	// 圧縮されてるものはサポート外
	if (this->bih.header_size != 40 || this->bih.compression != 0 || this->bih.bits != 24) {
		ifs.close();
		throw "サポート外のビットマップだよ";
	}
	
	// ビットをバイトに直す
	const unsigned int byte  = this->bih.bits / 8;
	
	// 一々打つのが面倒なので、コピーしておく
	const unsigned int width = this->bih.width;
	const unsigned int height = this->bih.height;
	
	// 何をしてるのかは11行目辺りを参照
	const unsigned int padding = (4 - (width*3)%4) & 0x03;
	
	// 全ピクセルに使うバイト数≠width*height*byte
	// paddingがあるのを忘れずに
	// 横ピクセル数*1ピクセルに使用するバイト数+paddingで横バイト数
	// それに縦の数をかける
	const unsigned int cDataSize = (width*byte + padding) * height;
	
	// ピクセルデータが保存されている一帯分のバッファ確保
	// 後述するようにゴミデータがあるので、これは一時データ
	unsigned char* buffer = new unsigned char[cDataSize];
	
	// 全ピクセルのデータを読み込む
	// ただし、11行目辺りに書いてあるように、このままだと
	// ゴミデータが入っていて使いづらい
	ifs.read((char*)buffer, sizeof(unsigned char)*cDataSize);
	
	// 用済みなので解放
	ifs.close();
	
	// 色データの保存場所を確保する
	this->color = new Color[width*height];
	
	const unsigned int height_max = height - 1;
	unsigned int buffer_pos, data_pos;
	
	// ビットマップは通常上下が反転して記録されている
	// なので、height_max - height によって
	// 下から逆順に読み込む
	// 実は、heightが負数のときは逆転していないのだけれど、
	// そういうデータはあまり使われていないようなので無視する
	// ここでゴミデータを飛ばすことで使いやすくする
	for (unsigned int x = 0; x < width; ++x) {
		for (unsigned int y = 0; y < height; ++y) {
			// データは下のような感じで収まっている
			// 
			// x→
			// ↑[00][01][02][03][04][05][06][07]
			// y [08][09][10][11][12][13][14][15]
			// 
			// なので、たとえば、座標(2, 1)の場合、上の例だと[01]にあたる
			// そこで、width*byte + paddingで横幅バイト数を数えて、
			// height_max - yをかけて、横幅分のデータを飛ばし、
			// それにx*byteを足すことで座標の位置を探す
			buffer_pos = (width*byte + padding)*(height_max - y) + x*byte;
			
			// 利用する方のデータはpaddingを利用しないので、
			// もっと素直な記述になっている
			data_pos = x + y*width;
			
			// データを代入していく
			// データは[青][緑][赤]の順に入っているので、
			// buffer_posが青、+1で緑、+2で赤となっていく
			color[data_pos].b = buffer[buffer_pos];
			color[data_pos].g = buffer[buffer_pos + 1];
			color[data_pos].r = buffer[buffer_pos + 2];
		}
	}
	
	// 用済みなので解放
	delete[] buffer;
}

MyBitmap::~MyBitmap()
{
	delete[] color;  // 配列の削除はdelete[]で
}

void MyBitmap::SaveFile(const char *const filename)
{
	std::ofstream ofs(filename, std::ios::binary);
	if (ofs.fail()) throw "ファイルが開けないよ";
	
	// ヘッダ書き込み
	// インスタンス生成時に出来たヘッダ情報を書けばOK
	ofs.write((char*)&this->bfh.type, sizeof(unsigned short));
	ofs.write((char*)&this->bfh.size, sizeof(unsigned long));
	ofs.write((char*)&this->bfh.reserved1, sizeof(unsigned short));
	ofs.write((char*)&this->bfh.reserved2, sizeof(unsigned short));
	ofs.write((char*)&this->bfh.offset, sizeof(unsigned long));
	
	// まだまだヘッダ書き込み
	ofs.write((char*)&this->bih.header_size, sizeof(unsigned long));
	ofs.write((char*)&this->bih.width, sizeof(long));
	ofs.write((char*)&this->bih.height, sizeof(long));
	ofs.write((char*)&this->bih.planes, sizeof(unsigned short));
	ofs.write((char*)&this->bih.bits, sizeof(unsigned short));
	ofs.write((char*)&this->bih.compression, sizeof(unsigned long));
	ofs.write((char*)&this->bih.data_size, sizeof(unsigned long));
	ofs.write((char*)&this->bih.x_resolution, sizeof(long));
	ofs.write((char*)&this->bih.y_resolution, sizeof(long));
	ofs.write((char*)&this->bih.colours, sizeof(unsigned long));
	ofs.write((char*)&this->bih.important_colour, sizeof(long));
	
	const unsigned int width = this->bih.width;
	const unsigned int height = this->bih.height;
	
	// 読み込みの方を参照
	const unsigned int byte = this->bih.bits / 8;
	const unsigned int padding = (4 - (width*3)%4) & 0x03;
	const unsigned int cDataSize = (width*byte + padding) * height;
	
	unsigned char* buffer = new unsigned char[cDataSize];
	unsigned int buffer_pos, data_pos;
	const unsigned int height_max = height - 1;
	
	// 後ろの方から読み込んで書き出し
	// 直接ファイルに書き込むのではなくて、
	// まずはバッファに書き込むデータを作成する
	for (unsigned int x = 0; x < width; ++x) {
		for (unsigned int y = 0; y < height; ++y) {
			// 今回はデータを後ろから読み出していく
			data_pos = x + (height_max - y)*width;
			
			// データを後ろから読み出しているので、
			// バッファは前から書き込んでいく
			buffer_pos = (width*byte + padding)*y + x*byte;
			
			buffer[buffer_pos] = color[data_pos].b;
			buffer[buffer_pos + 1] = color[data_pos].g;
			buffer[buffer_pos + 2] = color[data_pos].r;
		}
	}
	
	// 完成したデータをファイルに書き込み
	ofs.write((char*)buffer, sizeof(unsigned char)*cDataSize);
	ofs.close();
	
	delete[] buffer;
}

int MyBitmap::GetWidth()
{
	return this->bih.width;
}

int MyBitmap::GetHeight()
{
	return this->bih.height;
}

void MyBitmap::SetRed(int x, int y, int red)
{
	// あらぬ場所を求めている場合は何もしない
	if (x >= this->bih.width || x < 0 || y >= this->bih.height || y < 0) return;
	
	// 色は0～255の間でなければならない
	// それ以外の場合、最も近い値に修正する
	if (red > 255) red = 255;
	else if (red < 0) red = 0;
	
	this->color[x + y*this->bih.width].r = red;
}

void MyBitmap::SetGreen(int x, int y, int green)
{
	if (x >= this->bih.width || x < 0 || y >= this->bih.height || y < 0) return;
	if (green > 255) green = 255;
	else if (green < 0) green = 0;
	
	this->color[x + y*this->bih.width].g = green;
}

void MyBitmap::SetBlue(int x, int y, int blue)
{
	if (x >= this->bih.width || x < 0 || y >= this->bih.height || y < 0) return;
	if (blue > 255) blue = 255;
	else if (blue < 0) blue = 0;
	
	this->color[x + y*this->bih.width].b = blue;
}

int MyBitmap::GetRed(int x, int y)
{
	// あらぬ場所を求めている場合には、-1（色値としてはあり得ない色）を返す
	if (x >= this->bih.width || x < 0 || y >= this->bih.height || y < 0) return -1;
	return this->color[x + y*this->bih.width].r;
}

int MyBitmap::GetGreen(int x, int y)
{
	if (x >= this->bih.width || x < 0 || y >= this->bih.height || y < 0) return -1;
	return this->color[x + y*this->bih.width].g;
}

int MyBitmap::GetBlue(int x, int y)
{
	if (x >= this->bih.width || x < 0 || y >= this->bih.height || y < 0) return -1;
	return this->color[x + y*this->bih.width].b;
}
