#include "Bitmap.h"
#include <cstring>   // memset�ɕK�v
#include <fstream>   // ofstream��ifstream�ɕK�v

MyBitmap::MyBitmap(int x, int y)
{
	if (x <= 0) x = 1;  // �s���ȃT�C�Y��������A�Ƃ肠����1�s�N�Z���ɂ��Ă��܂�
	if (y <= 0) y = 1;
	std::memset(&this->bfh, 0x00, sizeof(MyBitmap::BITMAPFILEHEADER));  // �ȈՏ�����
	std::memset(&this->bih, 0x00, sizeof(MyBitmap::BITMAPINFOHEADER));
	
	// �r�b�g�}�b�v�t�@�C���ł́A������4�̔{���o�C�g�ƒ�`�����̂ŁA
	// ���r���[�ȉ����̏ꍇ�A�S�~�f�[�^�������ɒu�����
	// �����ŁA�S�~�f�[�^�̗ʂ𒲂ׂ�K�v������
	// �����ł́Ax*3�Ƃ��邱�ƂŁA�K�v�Ƃ���鉡���o�C�g���𒲂ׂ���A
	// 4�Ŋ������]���4����������ƂŁA�s�����̃o�C�g���i�S�~�f�[�^�ʁj�𒲂ׂ�
	// ����ǂ��A���̂܂܂��Ɨ]��0���s��0�̂Ƃ�4�ɂȂ��Ă��܂�
	// �Ƃ���ŁA0�`4�܂ł̓�i���\����
	// 0��000
	// 1��001
	// 2��010
	// 3��011
	// 4��100
	// �Ȃ̂ŁA��2���������o����Β��낪����������
	// �����ŁA& 0x03�i0x03�͓�i����011�Ɠ����j�Ƃ���āA
	// ���񌅕������������o��
	const unsigned int padding = (4 - (x*3)%4) & 0x03;
	
	// �w�b�_������������
	// �����ŏ������܂Ȃ��������̂�0��OK
	this->bfh.type = 0x4d42;  // BMP�t�@�C���ł���[�Ƃ����Ӗ�
	this->bfh.size = 14 + 40 + (x*3 + padding)*y;  // �摜�t�@�C���̃T�C�Y
	this->bfh.offset = 14 + 40;  // ���������o�Ă�������ǂ��A����͉摜�̃w�b�_�T�C�Y
	
	// ���̃w�b�_�̃T�C�Y�c�c�Ȃ̂�����ǂ��A
	// �R���p�C���ɂ���Ă͏���ɍœK�����ăT�C�Y�����������Ȃ�̂ŁA
	// ���ڎw�肵�Ă����Ă���
	this->bih.header_size = 40;
	this->bih.width = x;
	this->bih.height = y;
	// �g�p����v���[���Ƃ��������̂��߂�炵�����A���1��OK
	// ���Ԃ�A0���ꎟ���A1���񎟌�
	this->bih.planes = 1;
	this->bih.bits = 24;  // 24�r�b�g�}�b�v�ł���[�Ƃ�������
	
	this->color = new Color[x*y];  // �s�N�Z�������̔z������
	
	// ���ŏ�����
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
	
	// ifstream��Input File Stream�̂��ƂŁA
	// ���́i�ǂݍ��݁j�p�̃X�g���[���N���X
	std::ifstream ifs(filename, std::ios::binary);  // �o�C�i���Ƃ��ĊJ��
	if (ifs.fail()) throw "�t�@�C�����J���Ȃ���";  // �t�@�C�����J���̂Ɏ��s
	
	// �r�b�g�}�b�v�w�b�_��񂪈ُ�
	ifs.read((char*)&this->bfh.type, sizeof(unsigned short));
	if (this->bfh.type != 0x4d42) {
		ifs.close();
		throw "�r�b�g�}�b�v�t�@�C������Ȃ���";
	}
	
	
	// �w�b�_�ǂݍ���
	// �ׁX�Ɠǂݍ���ł�̂́A�R���p�C�����\���̂��œK������
	// ���ȂƂ���Ƀf�[�^������Ɗ�Ȃ��̂�
	// �w�b�_���̈Ӗ��͂Ƃ肠������������OK
	
	// �t�@�C���S�̂̃T�C�Y������
	ifs.read((char*)&this->bfh.size, sizeof(unsigned long));
	ifs.read((char*)&this->bfh.reserved1, sizeof(unsigned short));
	ifs.read((char*)&this->bfh.reserved2, sizeof(unsigned short));
	ifs.read((char*)&this->bfh.offset, sizeof(unsigned long));
	
	// �܂��܂��w�b�_�ǂݍ���
	ifs.read((char*)&this->bih.header_size, sizeof(unsigned long));
	
	// �摜�̏c���T�C�Y
	// ���������͒m���Ă����Ɨǂ�
	ifs.read((char*)&this->bih.width, sizeof(long));
	ifs.read((char*)&this->bih.height, sizeof(long));
	ifs.read((char*)&this->bih.planes, sizeof(unsigned short));
	ifs.read((char*)&this->bih.bits, sizeof(unsigned short));
	ifs.read((char*)&this->bih.compression, sizeof(unsigned long));
	
	// ����͎g��Ȃ�����ǂ��A
	// �w�b�_���������t�@�C���T�C�Y��������Ă���
	ifs.read((char*)&this->bih.data_size, sizeof(unsigned long));
	ifs.read((char*)&this->bih.x_resolution, sizeof(long));
	ifs.read((char*)&this->bih.y_resolution, sizeof(long));
	ifs.read((char*)&this->bih.colours, sizeof(unsigned long));
	ifs.read((char*)&this->bih.important_colour, sizeof(long));
	
	// �}�C�i�[�w�b�_��������A24�r�b�g�}�b�v����Ȃ�������A
	// ���k����Ă���̂̓T�|�[�g�O
	if (this->bih.header_size != 40 || this->bih.compression != 0 || this->bih.bits != 24) {
		ifs.close();
		throw "�T�|�[�g�O�̃r�b�g�}�b�v����";
	}
	
	// �r�b�g���o�C�g�ɒ���
	const unsigned int byte  = this->bih.bits / 8;
	
	// ��X�ł̂��ʓ|�Ȃ̂ŁA�R�s�[���Ă���
	const unsigned int width = this->bih.width;
	const unsigned int height = this->bih.height;
	
	// �������Ă�̂���11�s�ڕӂ���Q��
	const unsigned int padding = (4 - (width*3)%4) & 0x03;
	
	// �S�s�N�Z���Ɏg���o�C�g����width*height*byte
	// padding������̂�Y�ꂸ��
	// ���s�N�Z����*1�s�N�Z���Ɏg�p����o�C�g��+padding�ŉ��o�C�g��
	// ����ɏc�̐���������
	const unsigned int cDataSize = (width*byte + padding) * height;
	
	// �s�N�Z���f�[�^���ۑ�����Ă����ѕ��̃o�b�t�@�m��
	// ��q����悤�ɃS�~�f�[�^������̂ŁA����͈ꎞ�f�[�^
	unsigned char* buffer = new unsigned char[cDataSize];
	
	// �S�s�N�Z���̃f�[�^��ǂݍ���
	// �������A11�s�ڕӂ�ɏ����Ă���悤�ɁA���̂܂܂���
	// �S�~�f�[�^�������Ă��Ďg���Â炢
	ifs.read((char*)buffer, sizeof(unsigned char)*cDataSize);
	
	// �p�ς݂Ȃ̂ŉ��
	ifs.close();
	
	// �F�f�[�^�̕ۑ��ꏊ���m�ۂ���
	this->color = new Color[width*height];
	
	const unsigned int height_max = height - 1;
	unsigned int buffer_pos, data_pos;
	
	// �r�b�g�}�b�v�͒ʏ�㉺�����]���ċL�^����Ă���
	// �Ȃ̂ŁAheight_max - height �ɂ����
	// ������t���ɓǂݍ���
	// ���́Aheight�������̂Ƃ��͋t�]���Ă��Ȃ��̂�����ǁA
	// ���������f�[�^�͂��܂�g���Ă��Ȃ��悤�Ȃ̂Ŗ�������
	// �����ŃS�~�f�[�^���΂����ƂŎg���₷������
	for (unsigned int x = 0; x < width; ++x) {
		for (unsigned int y = 0; y < height; ++y) {
			// �f�[�^�͉��̂悤�Ȋ����Ŏ��܂��Ă���
			// 
			// x��
			// ��[00][01][02][03][04][05][06][07]
			// y [08][09][10][11][12][13][14][15]
			// 
			// �Ȃ̂ŁA���Ƃ��΁A���W(2, 1)�̏ꍇ�A��̗Ⴞ��[01]�ɂ�����
			// �����ŁAwidth*byte + padding�ŉ����o�C�g���𐔂��āA
			// height_max - y�������āA�������̃f�[�^���΂��A
			// �����x*byte�𑫂����Ƃō��W�̈ʒu��T��
			buffer_pos = (width*byte + padding)*(height_max - y) + x*byte;
			
			// ���p������̃f�[�^��padding�𗘗p���Ȃ��̂ŁA
			// �����Ƒf���ȋL�q�ɂȂ��Ă���
			data_pos = x + y*width;
			
			// �f�[�^�������Ă���
			// �f�[�^��[��][��][��]�̏��ɓ����Ă���̂ŁA
			// buffer_pos���A+1�ŗ΁A+2�ŐԂƂȂ��Ă���
			color[data_pos].b = buffer[buffer_pos];
			color[data_pos].g = buffer[buffer_pos + 1];
			color[data_pos].r = buffer[buffer_pos + 2];
		}
	}
	
	// �p�ς݂Ȃ̂ŉ��
	delete[] buffer;
}

MyBitmap::~MyBitmap()
{
	delete[] color;  // �z��̍폜��delete[]��
}

void MyBitmap::SaveFile(const char *const filename)
{
	std::ofstream ofs(filename, std::ios::binary);
	if (ofs.fail()) throw "�t�@�C�����J���Ȃ���";
	
	// �w�b�_��������
	// �C���X�^���X�������ɏo�����w�b�_����������OK
	ofs.write((char*)&this->bfh.type, sizeof(unsigned short));
	ofs.write((char*)&this->bfh.size, sizeof(unsigned long));
	ofs.write((char*)&this->bfh.reserved1, sizeof(unsigned short));
	ofs.write((char*)&this->bfh.reserved2, sizeof(unsigned short));
	ofs.write((char*)&this->bfh.offset, sizeof(unsigned long));
	
	// �܂��܂��w�b�_��������
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
	
	// �ǂݍ��݂̕����Q��
	const unsigned int byte = this->bih.bits / 8;
	const unsigned int padding = (4 - (width*3)%4) & 0x03;
	const unsigned int cDataSize = (width*byte + padding) * height;
	
	unsigned char* buffer = new unsigned char[cDataSize];
	unsigned int buffer_pos, data_pos;
	const unsigned int height_max = height - 1;
	
	// ���̕�����ǂݍ���ŏ����o��
	// ���ڃt�@�C���ɏ������ނ̂ł͂Ȃ��āA
	// �܂��̓o�b�t�@�ɏ������ރf�[�^���쐬����
	for (unsigned int x = 0; x < width; ++x) {
		for (unsigned int y = 0; y < height; ++y) {
			// ����̓f�[�^����납��ǂݏo���Ă���
			data_pos = x + (height_max - y)*width;
			
			// �f�[�^����납��ǂݏo���Ă���̂ŁA
			// �o�b�t�@�͑O���珑������ł���
			buffer_pos = (width*byte + padding)*y + x*byte;
			
			buffer[buffer_pos] = color[data_pos].b;
			buffer[buffer_pos + 1] = color[data_pos].g;
			buffer[buffer_pos + 2] = color[data_pos].r;
		}
	}
	
	// ���������f�[�^���t�@�C���ɏ�������
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
	// ����ʏꏊ�����߂Ă���ꍇ�͉������Ȃ�
	if (x >= this->bih.width || x < 0 || y >= this->bih.height || y < 0) return;
	
	// �F��0�`255�̊ԂłȂ���΂Ȃ�Ȃ�
	// ����ȊO�̏ꍇ�A�ł��߂��l�ɏC������
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
	// ����ʏꏊ�����߂Ă���ꍇ�ɂ́A-1�i�F�l�Ƃ��Ă͂��蓾�Ȃ��F�j��Ԃ�
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
