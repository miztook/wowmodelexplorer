#include "mywow.h"
#include "CFileSystem.h"

#pragma comment(lib, "mywow.lib")

IFileSystem* g_fs = NULL;
wowEnvironment* g_wowEnv = NULL;

void g_callbackDBC(const c8* filename, void* param);
void g_callbackDB2(const c8* filename, void* param);

void g_callbackDBC1(const c8* filename, void* param);

void g_callbackFile(const c8* filename, void* param);

int main()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	initGlobal();

	QMem_Init(0, 30, 40);

	g_fs = new CFileSystem("", "", false);
	g_wowEnv = new wowEnvironment(g_fs, true, false);

	/*Q_iterateFiles("C:\\Users\\miaoyu\\Desktop\\Work\\unknown", "*.*", g_callbackFile, NULL, "C:\\Users\\miaoyu\\Desktop\\Work\\unknown");*/

	//g_wowEnv->iterateFiles("dbc", g_callbackDBC, g_wowEnv);

	g_wowEnv->iterateFiles("db2", g_callbackDB2, g_wowEnv);

// 	g_callbackDB2("DBFilesClient\\SpellVisualEffectName.db2", g_wowEnv);
// 	g_callbackDB2("DBFilesClient\\SpellVisualKit.db2", g_wowEnv);
// 	g_callbackDB2("DBFilesClient\\SpellVisual.db2", g_wowEnv);

//	g_wowEnv->iterateFiles("dbc", g_callbackDBC1, g_wowEnv);

	delete g_wowEnv;
	delete g_fs;

	QMem_End();

	deleteGlobal();

	getchar();
	return 0;
}

void g_callbackFile(const c8* filename, void* param)
{
	string512 strFileName = "C:\\Users\\miaoyu\\Desktop\\Work\\unknown";
	strFileName.normalizeDir();
	strFileName.append(filename);
	FILE* file = fopen(strFileName.c_str(), "rb");
	
	char tmp[4];
	fread(tmp, 1, 4, file);
	const c8* magic = (const c8*)tmp;
	if (strncmp(magic, "WDB5", 4) == 0)
	{
		int x = 0;
	}

	fclose(file);

	printf("%s\n", filename);
}

void g_callbackDBC1(const c8* filename, void* param)
{
	if (strstr(filename, "internal") || strstr(filename, "wmominimaptexture"))
	{
		return;
	}

	if(!((wowEnvironment*)param)->exists(filename))
		return;

	dbc* db = new dbc((wowEnvironment*)param, filename, true);

	if (db->getStringSize() <= 1)
	{
		delete db;
		return;
	}

	c8* buf = (c8*)Z_AllocateTempMemory(db->getStringSize() + 1);
	memcpy_s(buf, db->getStringSize()+1, db->getStringStart(), db->getStringSize());
	buf[db->getStringSize()] = '\0';

	char *p=buf;
	while (p<buf+db->getStringSize()) {

		size_t len = strlen(p);
		if (len < 256)
		{
			string256 str = p;
			//utf8to16(str, tmp, 1024);
			str.make_lower();
			if (strstr(str.c_str(), "humanmaleskin"))
			{
				int x = 0;
			}
		}
		else
		{
			const char* str = p;

			int x = 0;
		}
	
		p+=strlen(p)+1;
	}
	Z_FreeTempMemory(buf);

	delete db;
}

void g_callbackDBC(const c8* filename, void* param)
{
	if (strstr(filename, "internal"))
		return;

	if(!((wowEnvironment*)param)->exists(filename))
		return;

	printf("reading: %s\n", filename);
	
	string256 path = g_fs->getBaseDirectory();
	path.append("dbc\\");

	c8 name[DEFAULT_SIZE];
	getFileNameA(filename, name, DEFAULT_SIZE);
	getFileNameNoExtensionA(name, name, DEFAULT_SIZE);
	path.append(name);
	path.append(".txt");

	IWriteFile* file = g_fs->createAndWriteFile(path.c_str(), false);
	_ASSERT(file);

	dbc* db = new dbc((wowEnvironment*)param, filename, true);

	c8 txt[1024];
	sprintf_s(txt, 1024, "记录数: %d\n", db->getNumActualRecords());
	file->writeText(txt);
	sprintf_s(txt, 1024, "记录字段数: %d\n", db->getNumFields());
	file->writeText(txt);
	sprintf_s(txt, 1024, "记录大小: %d\n", db->getRecordSize());
	file->writeText(txt);
	sprintf_s(txt, 1024, "字符串区域大小: %d\n", db->getStringSize());
	file->writeText(txt);

	if (db->getRecordSize() == db->getNumFields() * 4)
	{
		for (u32 i=0; i<db->getNumActualRecords(); ++i)
		{
			dbc::record r = db->getRecord(i);

			sprintf_s(txt, 1024, "记录%d:", i);

			for (u32 k=0; k<db->getNumFields(); ++k)
			{
				c8 tmp[32];
				sprintf_s(tmp, 32, "\t%d", r.getInt(k));
				strcat_s(txt, 1024, tmp);
			}

			strcat_s(txt, 1024, "\n");

			file->writeText(txt);
		}
	}

	bool writeString = db->getStringSize() > 1;
	if (writeString)
	{
		sprintf_s(txt, 1024, "字符串: \n");
		file->writeText(txt);
	}

	c8* buf = (c8*)Z_AllocateTempMemory(db->getStringSize() + 1);
	memcpy_s(buf, db->getStringSize()+1, db->getStringStart(), db->getStringSize());
	buf[db->getStringSize()] = '\0';

	c16 tmp[1024];
	c8 outText[1000];

	char *p=buf;
	while (p<buf+db->getStringSize()) {

		const c8* str = p;
		utf8to16(str, tmp, 1024);
	
		if (writeString)
		{
			utf16_to_gbk(tmp, outText, 1000);
			sprintf_s(txt, 1024, "offset: %d	%s\n", p-buf, outText);
			file->writeText(txt);
		}

		p+=strlen(p)+1;
	}
	Z_FreeTempMemory(buf);

	delete db;

	delete file;
}

void g_callbackDB2(const c8* filename, void* param)
{
	if (strstr(filename, "internal") || strstr(filename, "wmominimaptexture"))
	{
		return;
	}

	if(!((wowEnvironment*)param)->exists(filename))
		return;

	printf("reading: %s\n", filename);

	string256 path = g_fs->getBaseDirectory();
	path.append("db2\\");

	c8 name[DEFAULT_SIZE];
	getFileNameA(filename, name, DEFAULT_SIZE);
	getFileNameNoExtensionA(name, name, DEFAULT_SIZE);
	path.append(name);
	path.append(".txt");

	IWriteFile* file = g_fs->createAndWriteFile(path.c_str(), false);
	_ASSERT(file);

	dbc* db = new dbc((wowEnvironment*)param, filename, true);

	c8 txt[1024];
	sprintf_s(txt, 1024, "记录数: %d\n", db->getNumActualRecords());
	file->writeText(txt);
	sprintf_s(txt, 1024, "记录字段数: %d\n", db->getNumFields());
	file->writeText(txt);
	sprintf_s(txt, 1024, "记录大小: %d\n", db->getRecordSize());
	file->writeText(txt);
	sprintf_s(txt, 1024, "字符串区域大小: %d\n", db->getStringSize());
	file->writeText(txt);

	for (u32 i=0; i<db->getNumActualRecords(); ++i)
	{
		dbc::record r = db->getRecord(i);

		sprintf_s(txt, 1024, "记录%d:", i);

		if (db->hasIndex())
		{
			c8 tmp[32];
			sprintf_s(tmp, 32, "\t[%d]", db->getIDs()[i]);
			strcat_s(txt, 1024, tmp);
		}

		for (u32 k=0; k<db->getNumFields(); ++k)
		{
			u16 fs = db->getFieldSize(k);

			c8 tmp[32] = {0};
			if(fs == sizeof(int) || fs == sizeof(u16) || fs == sizeof(u8))
				sprintf_s(tmp, 32, "\t%d", r.getUInt(k));
			else
			{
				const char* str = (const char*)r.getRawPointer<char>(k);
				strncpy(tmp, str, fs);
			}
			strcat_s(txt, 1024, tmp);
		}

		strcat_s(txt, 1024, "\n");

		file->writeText(txt);
	}
	
	bool writeString = db->getStringSize() > 1;
	if (writeString)
	{
		sprintf_s(txt, 1024, "字符串: \n");
		file->writeText(txt);
	}

	c8* buf = (c8*)Z_AllocateTempMemory(db->getStringSize() + 1);
	memcpy_s(buf, db->getStringSize()+1, db->getStringStart(), db->getStringSize());
	buf[db->getStringSize()] = '\0';

	c16 tmp[1024];
	c8 outText[1000];

	char *p=buf;
	while (p<buf+db->getStringSize()) {

		const c8* str = p;
		utf8to16(str, tmp, 1024);

		if (writeString)
		{
			utf16_to_gbk(tmp, outText, 1000);
			sprintf_s(txt, 1024, "offset: %d	%s\n", p-buf, outText);
			file->writeText(txt);
		}

		p+=strlen(p)+1;
	}
	Z_FreeTempMemory(buf);

	delete db;

	delete file;
}