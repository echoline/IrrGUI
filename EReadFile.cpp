#include "dat.h"

EReadFile::EReadFile(const io::path &n) {
	name = n;
}

s32 EReadFile::read(void *buf, u32 len) {
}

bool EReadFile::seek(long int pos, bool rel) {
	return false;
}

long int EReadFile::getSize() const {
	return -1;
}

long int EReadFile::getPos() const {
	return 0;
}

const io::path& EReadFile::getFileName() const {
	return name;
}

