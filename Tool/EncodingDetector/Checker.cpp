#include "Checker.h"
#include "mywow.h"

extern IFileSystem* g_fs;

extern char g_leftDir[QMAX_PATH];
extern char g_rightDir[QMAX_PATH];

extern bool g_SkipMissingFile;

void checkEncodingScript()
{
	string512 leftdir = g_leftDir;
	leftdir.normalizeDir();
	leftdir.append("script");

	string512 rightDir = g_rightDir;
	rightDir.normalizeDir();
	rightDir.append("script");

	Q_iterateFiles(leftdir.c_str(), "*.*", funcCheckEncodingScript, NULL);

	printf("Script编码比较完成!\n");
	printf("\n");
}

void funcCheckEncodingScript( const c8* filename, void* args )
{
	c8 ext[10];
	getFileExtensionA(filename, ext, 10);
	if (Q_stricmp(ext, "lua") != 0)
		return;

	c8 rightfilename[QMAX_PATH];
	const c8* c = strstr(filename, g_leftDir);
	_ASSERT(c);
	c += strlen(g_leftDir);
	Q_strcpy(rightfilename, QMAX_PATH, g_rightDir);
	Q_strcat(rightfilename, QMAX_PATH, c);

	E_UniMode leftMode = (E_UniMode)-1;
	E_UniMode rightMode = (E_UniMode)-1;

	{
		IReadFile* rfile = g_fs->createAndOpenFile(filename, false);

		u8* buffer = (u8*)Z_AllocateTempMemory(rfile->getSize());

		u32 read = rfile->read(buffer, rfile->getSize());
		_ASSERT(read <= rfile->getSize());

		leftMode = determineEncoding(buffer, rfile->getSize());

		Z_FreeTempMemory(buffer);

		delete rfile;
	}

	{
		IReadFile* rfile = g_fs->createAndOpenFile(rightfilename, false);

		if (!rfile)
		{
			if (!g_SkipMissingFile)
			{
				printf("文件无法打开 %s\n", rightfilename);
				getchar();
			}
			return;
		}

		u8* buffer = (u8*)Z_AllocateTempMemory(rfile->getSize());

		u32 read = rfile->read(buffer, rfile->getSize());
		_ASSERT(read <= rfile->getSize());

		rightMode = determineEncoding(buffer, rfile->getSize());

		Z_FreeTempMemory(buffer);

		delete rfile;
	}

	if (leftMode != rightMode)
	{
		printf("文件编码格式不同，转换后文件 %s\n", rightfilename);
		getchar();
	}
	else
	{
		printf("检查成功 %s\n", rightfilename);
	}
}

void checkEncodingInterface()
{
	string512 leftdir = g_leftDir;
	leftdir.normalizeDir();
	leftdir.append("interfaces");

	string512 rightDir = g_rightDir;
	rightDir.normalizeDir();
	rightDir.append("interfaces");

	Q_iterateFiles(leftdir.c_str(), "*.*", funcCheckEncodingInterface, NULL);

	printf("Interfaces编码比较完成!\n");
	printf("\n");
}

void funcCheckEncodingInterface( const c8* filename, void* args )
{
	c8 ext[10];
	getFileExtensionA(filename, ext, 10);
	if (Q_stricmp(ext, "xml") != 0)
		return;

	c8 rightfilename[QMAX_PATH];
	const c8* c = strstr(filename, g_leftDir);
	_ASSERT(c);
	c += strlen(g_leftDir);
	Q_strcpy(rightfilename, QMAX_PATH, g_rightDir);
	Q_strcat(rightfilename, QMAX_PATH, c);

	E_UniMode leftMode = (E_UniMode)-1;
	E_UniMode rightMode = (E_UniMode)-1;

	{
		IReadFile* rfile = g_fs->createAndOpenFile(filename, false);

		u8* buffer = (u8*)Z_AllocateTempMemory(rfile->getSize());

		u32 read = rfile->read(buffer, rfile->getSize());
		_ASSERT(read <= rfile->getSize());

		leftMode = determineEncoding(buffer, rfile->getSize());

		Z_FreeTempMemory(buffer);

		delete rfile;
	}

	{
		IReadFile* rfile = g_fs->createAndOpenFile(rightfilename, false);

		if (!rfile)
		{
			if (!g_SkipMissingFile)
			{
				printf("文件无法打开 %s\n", rightfilename);
				getchar();
			}
			return;
		}

		u8* buffer = (u8*)Z_AllocateTempMemory(rfile->getSize());

		u32 read = rfile->read(buffer, rfile->getSize());
		_ASSERT(read <= rfile->getSize());

		rightMode = determineEncoding(buffer, rfile->getSize());

		Z_FreeTempMemory(buffer);

		delete rfile;
	}

	if (leftMode != rightMode)
	{
		printf("文件编码格式不同，转换后文件 %s\n", rightfilename);
		getchar();
	}
	else
	{
		printf("检查成功 %s\n", rightfilename);
	}
}

void checkEncodingSurfaces()
{
	string512 leftdir = g_leftDir;
	leftdir.normalizeDir();
	leftdir.append("surfaces");

	string512 rightDir = g_rightDir;
	rightDir.normalizeDir();
	rightDir.append("surfaces");

	Q_iterateFiles(leftdir.c_str(), "*.*", funcCheckEncodingSurfaces, NULL);

	printf("Surfaces编码比较完成!\n");
	printf("\n");
}

void funcCheckEncodingSurfaces( const c8* filename, void* args )
{
	c8 ext[10];
	getFileExtensionA(filename, ext, 10);
	if (Q_stricmp(ext, "txt") != 0)
		return;

	c8 rightfilename[QMAX_PATH];
	const c8* c = strstr(filename, g_leftDir);
	_ASSERT(c);
	c += strlen(g_leftDir);
	Q_strcpy(rightfilename, QMAX_PATH, g_rightDir);
	Q_strcat(rightfilename, QMAX_PATH, c);

	E_UniMode leftMode = (E_UniMode)-1;
	E_UniMode rightMode = (E_UniMode)-1;

	{
		IReadFile* rfile = g_fs->createAndOpenFile(filename, false);

		u8* buffer = (u8*)Z_AllocateTempMemory(rfile->getSize());

		u32 read = rfile->read(buffer, rfile->getSize());
		_ASSERT(read <= rfile->getSize());

		leftMode = determineEncoding(buffer, rfile->getSize());

		Z_FreeTempMemory(buffer);

		delete rfile;
	}

	{
		IReadFile* rfile = g_fs->createAndOpenFile(rightfilename, false);

		if (!rfile)
		{
			if (!g_SkipMissingFile)
			{
				printf("文件无法打开 %s\n", rightfilename);
				getchar();
			}
			return;
		}

		u8* buffer = (u8*)Z_AllocateTempMemory(rfile->getSize());

		u32 read = rfile->read(buffer, rfile->getSize());
		_ASSERT(read <= rfile->getSize());

		rightMode = determineEncoding(buffer, rfile->getSize());

		Z_FreeTempMemory(buffer);

		delete rfile;
	}

	if (leftMode != rightMode)
	{
		printf("文件编码格式不同，转换后文件 %s\n", rightfilename);
		getchar();
	}
	else
	{
		printf("检查成功 %s\n", rightfilename);
	}
}

void checkImageFileSize()
{
	string512 leftdir = g_leftDir;
	leftdir.normalizeDir();

	string512 rightDir = g_rightDir;
	rightDir.normalizeDir();

	Q_iterateFiles(leftdir.c_str(), "*.*", funcCheckImageSize, NULL);

	printf("图片大小比较完成!\n");
	printf("\n");
}

void funcCheckImageSize( const c8* filename, void* args )
{
	c8 ext[10];
	getFileExtensionA(filename, ext, 10);
	if (Q_stricmp(ext, "ktx") != 0)
		return;

	c8 rightfilename[QMAX_PATH];
	const c8* c = strstr(filename, g_leftDir);
	_ASSERT(c);
	c += strlen(g_leftDir);
	Q_strcpy(rightfilename, QMAX_PATH, g_rightDir);
	Q_strcat(rightfilename, QMAX_PATH, c);

	u32 leftSize = 0;
	u32 rightSize = 0;

	{
		IReadFile* rfile = g_fs->createAndOpenFile(filename, true);

		leftSize = rfile->getSize();

		delete rfile;
	}

	{
		IReadFile* rfile = g_fs->createAndOpenFile(rightfilename, true);

		if (!rfile)
		{
			if (!g_SkipMissingFile)
			{
				printf("文件无法打开 %s\n", rightfilename);
				getchar();
			}
			return;
		}

		rightSize = rfile->getSize();

		delete rfile;
	}

	if (leftSize != rightSize)
	{
		printf("文件大小不同，转换后文件 %s\n", rightfilename);
		printf("左侧大小: %u, 右侧大小: %u\n", leftSize, rightSize);
		getchar();
	}
	else
	{
		printf("检查成功 %s\n", rightfilename);
	}
}

void checkScript()
{
	string512 leftdir = g_leftDir;
	leftdir.normalizeDir();
	leftdir.append("script");

	string512 rightDir = g_rightDir;
	rightDir.normalizeDir();
	rightDir.append("script");

	Q_iterateFiles(leftdir.c_str(), "*.*", funcCheckScript, NULL);

	printf("Script翻译检查完成!\n");
	printf("\n");
}

void funcCheckScript( const c8* filename, void* args )
{
	c8 ext[10];
	getFileExtensionA(filename, ext, 10);
	if (Q_stricmp(ext, "lua") != 0)
		return;

	c8 rightfilename[QMAX_PATH];
	const c8* c = strstr(filename, g_leftDir);
	_ASSERT(c);
	c += strlen(g_leftDir);
	Q_strcpy(rightfilename, QMAX_PATH, g_rightDir);
	Q_strcat(rightfilename, QMAX_PATH, c);

	{
		c8 buffer[4096];

		IReadFile* rfile = g_fs->createAndOpenFile(rightfilename, false);

		printf("file: %s\n", rightfilename);

		int line = 1;
		while(!rfile->isEof())
		{
			memset(buffer, 0, sizeof(buffer));

			u32 len = rfile->readLine(buffer, 4096);
			_ASSERT(len < 4095);

			//ecm
			if (strstr(buffer, ".ecm\""))
			{
				c16 utf16[4096];
				utf8to16(buffer, utf16, 4096);
				u32 len = utf16len(utf16);
				for (u32 i=0; i<len; ++i)
				{
					c16 c = utf16[i];
					if (c == L'-' &&
						(i+1<len) && utf16[i+1] == c)
					{
						break;
					}

					if (((c & 0xff00) != 0) &&
						(c < 0x4E00 || c > 0x9FbF))
					{
						printf("错误的翻译! 文件 %s 第 %d 行\n", filename, line);
						getchar();
						break;
					}
				}
			}

			if (strstr(buffer, ".gfx\""))
			{
				c16 utf16[4096];
				utf8to16(buffer, utf16, 4096);
				u32 len = utf16len(utf16);
				for (u32 i=0; i<len; ++i)
				{
					c16 c = utf16[i];

					if (c == L'-' &&
						(i+1<len) && utf16[i+1] == c)
					{
						break;
					}

					if (((c & 0xff00) != 0) &&
						(c < 0x4E00 || c > 0x9FbF))
					{
						printf("错误的翻译! 文件 %s 第 %d 行\n", filename, line);
						getchar();
						break;
					}
				}
			}

			if (strstr(buffer, ".ski\""))
			{
				c16 utf16[4096];
				utf8to16(buffer, utf16, 4096);
				u32 len = utf16len(utf16);
				for (u32 i=0; i<len; ++i)
				{
					c16 c = utf16[i];
					
					if (c == L'-' &&
						(i+1<len) && utf16[i+1] == c)
					{
						break;
					}

					if (((c & 0xff00) != 0) &&
						(c < 0x4E00 || c > 0x9FbF))
					{
						printf("错误的翻译! 文件 %s 第 %d 行\n", filename, line);
						getchar();
						break;
					}
				}
			}

			++line;
		}

		delete rfile;
	}
}

