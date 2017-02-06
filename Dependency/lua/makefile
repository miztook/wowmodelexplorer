#====================================
# define
#====================================
COMPILE_OPTIONS = -I./include  -O0 -g3 -Wall -c -fmessage-length=0 -finput-charset=GBK -MMD -MP

OBJECT_FILE_LIST = \
./make/lapi.o \
./make/lauxlib.o \
./make/lbaselib.o \
./make/lcode.o \
./make/ldblib.o \
./make/ldebug.o \
./make/ldo.o \
./make/ldump.o \
./make/lfunc.o \
./make/lgc.o \
./make/linit.o \
./make/liolib.o \
./make/llex.o \
./make/lmathlib.o \
./make/lmem.o \
./make/loadlib.o \
./make/lobject.o \
./make/lopcodes.o \
./make/loslib.o \
./make/lparser.o \
./make/lstate.o \
./make/lstring.o \
./make/lstrlib.o \
./make/ltable.o \
./make/ltablib.o \
./make/ltm.o \
./make/lua.o \
./make/luac.o \
./make/lundump.o \
./make/lvm.o \
./make/lzio.o \
./make/print.o 

#====================================
# target file
#====================================
./make/liblua.a : prepare $(OBJECT_FILE_LIST) 
	ar -rc ./make/liblua.a $(OBJECT_FILE_LIST);\
rm -f $(SolutionDir)$(Platform)/$(Configuration)/liblua.a;\
cp ./make/liblua.a $(SolutionDir)$(Platform)/$(Configuration)/liblua.a 


#====================================
# object files
#====================================
./make/lapi.o : src/lapi.c
	g++ $(COMPILE_OPTIONS) -MF"./make/lapi.d" -MT"./make/lapi.d" -o"./make/lapi.o" "src/lapi.c" 2>> ./make/result.txt 

./make/lauxlib.o : src/lauxlib.c
	g++ $(COMPILE_OPTIONS) -MF"./make/lauxlib.d" -MT"./make/lauxlib.d" -o"./make/lauxlib.o" "src/lauxlib.c" 2>> ./make/result.txt 

./make/lbaselib.o : src/lbaselib.c
	g++ $(COMPILE_OPTIONS) -MF"./make/lbaselib.d" -MT"./make/lbaselib.d" -o"./make/lbaselib.o" "src/lbaselib.c" 2>> ./make/result.txt 

./make/lcode.o : src/lcode.c
	g++ $(COMPILE_OPTIONS) -MF"./make/lcode.d" -MT"./make/lcode.d" -o"./make/lcode.o" "src/lcode.c" 2>> ./make/result.txt 

./make/ldblib.o : src/ldblib.c
	g++ $(COMPILE_OPTIONS) -MF"./make/ldblib.d" -MT"./make/ldblib.d" -o"./make/ldblib.o" "src/ldblib.c" 2>> ./make/result.txt 

./make/ldebug.o : src/ldebug.c
	g++ $(COMPILE_OPTIONS) -MF"./make/ldebug.d" -MT"./make/ldebug.d" -o"./make/ldebug.o" "src/ldebug.c" 2>> ./make/result.txt 

./make/ldo.o : src/ldo.c
	g++ $(COMPILE_OPTIONS) -MF"./make/ldo.d" -MT"./make/ldo.d" -o"./make/ldo.o" "src/ldo.c" 2>> ./make/result.txt 

./make/ldump.o : src/ldump.c
	g++ $(COMPILE_OPTIONS) -MF"./make/ldump.d" -MT"./make/ldump.d" -o"./make/ldump.o" "src/ldump.c" 2>> ./make/result.txt 

./make/lfunc.o : src/lfunc.c
	g++ $(COMPILE_OPTIONS) -MF"./make/lfunc.d" -MT"./make/lfunc.d" -o"./make/lfunc.o" "src/lfunc.c" 2>> ./make/result.txt 

./make/lgc.o : src/lgc.c
	g++ $(COMPILE_OPTIONS) -MF"./make/lgc.d" -MT"./make/lgc.d" -o"./make/lgc.o" "src/lgc.c" 2>> ./make/result.txt 

./make/linit.o : src/linit.c
	g++ $(COMPILE_OPTIONS) -MF"./make/linit.d" -MT"./make/linit.d" -o"./make/linit.o" "src/linit.c" 2>> ./make/result.txt 

./make/liolib.o : src/liolib.c
	g++ $(COMPILE_OPTIONS) -MF"./make/liolib.d" -MT"./make/liolib.d" -o"./make/liolib.o" "src/liolib.c" 2>> ./make/result.txt 

./make/llex.o : src/llex.c
	g++ $(COMPILE_OPTIONS) -MF"./make/llex.d" -MT"./make/llex.d" -o"./make/llex.o" "src/llex.c" 2>> ./make/result.txt 

./make/lmathlib.o : src/lmathlib.c
	g++ $(COMPILE_OPTIONS) -MF"./make/lmathlib.d" -MT"./make/lmathlib.d" -o"./make/lmathlib.o" "src/lmathlib.c" 2>> ./make/result.txt 

./make/lmem.o : src/lmem.c
	g++ $(COMPILE_OPTIONS) -MF"./make/lmem.d" -MT"./make/lmem.d" -o"./make/lmem.o" "src/lmem.c" 2>> ./make/result.txt 

./make/loadlib.o : src/loadlib.c
	g++ $(COMPILE_OPTIONS) -MF"./make/loadlib.d" -MT"./make/loadlib.d" -o"./make/loadlib.o" "src/loadlib.c" 2>> ./make/result.txt 

./make/lobject.o : src/lobject.c
	g++ $(COMPILE_OPTIONS) -MF"./make/lobject.d" -MT"./make/lobject.d" -o"./make/lobject.o" "src/lobject.c" 2>> ./make/result.txt 

./make/lopcodes.o : src/lopcodes.c
	g++ $(COMPILE_OPTIONS) -MF"./make/lopcodes.d" -MT"./make/lopcodes.d" -o"./make/lopcodes.o" "src/lopcodes.c" 2>> ./make/result.txt 

./make/loslib.o : src/loslib.c
	g++ $(COMPILE_OPTIONS) -MF"./make/loslib.d" -MT"./make/loslib.d" -o"./make/loslib.o" "src/loslib.c" 2>> ./make/result.txt 

./make/lparser.o : src/lparser.c
	g++ $(COMPILE_OPTIONS) -MF"./make/lparser.d" -MT"./make/lparser.d" -o"./make/lparser.o" "src/lparser.c" 2>> ./make/result.txt 

./make/lstate.o : src/lstate.c
	g++ $(COMPILE_OPTIONS) -MF"./make/lstate.d" -MT"./make/lstate.d" -o"./make/lstate.o" "src/lstate.c" 2>> ./make/result.txt 

./make/lstring.o : src/lstring.c
	g++ $(COMPILE_OPTIONS) -MF"./make/lstring.d" -MT"./make/lstring.d" -o"./make/lstring.o" "src/lstring.c" 2>> ./make/result.txt 

./make/lstrlib.o : src/lstrlib.c
	g++ $(COMPILE_OPTIONS) -MF"./make/lstrlib.d" -MT"./make/lstrlib.d" -o"./make/lstrlib.o" "src/lstrlib.c" 2>> ./make/result.txt 

./make/ltable.o : src/ltable.c
	g++ $(COMPILE_OPTIONS) -MF"./make/ltable.d" -MT"./make/ltable.d" -o"./make/ltable.o" "src/ltable.c" 2>> ./make/result.txt 

./make/ltablib.o : src/ltablib.c
	g++ $(COMPILE_OPTIONS) -MF"./make/ltablib.d" -MT"./make/ltablib.d" -o"./make/ltablib.o" "src/ltablib.c" 2>> ./make/result.txt 

./make/ltm.o : src/ltm.c
	g++ $(COMPILE_OPTIONS) -MF"./make/ltm.d" -MT"./make/ltm.d" -o"./make/ltm.o" "src/ltm.c" 2>> ./make/result.txt 

./make/lua.o : src/lua.c
	g++ $(COMPILE_OPTIONS) -MF"./make/lua.d" -MT"./make/lua.d" -o"./make/lua.o" "src/lua.c" 2>> ./make/result.txt 

./make/luac.o : src/luac.c
	g++ $(COMPILE_OPTIONS) -MF"./make/luac.d" -MT"./make/luac.d" -o"./make/luac.o" "src/luac.c" 2>> ./make/result.txt 

./make/lundump.o : src/lundump.c
	g++ $(COMPILE_OPTIONS) -MF"./make/lundump.d" -MT"./make/lundump.d" -o"./make/lundump.o" "src/lundump.c" 2>> ./make/result.txt 

./make/lvm.o : src/lvm.c
	g++ $(COMPILE_OPTIONS) -MF"./make/lvm.d" -MT"./make/lvm.d" -o"./make/lvm.o" "src/lvm.c" 2>> ./make/result.txt 

./make/lzio.o : src/lzio.c
	g++ $(COMPILE_OPTIONS) -MF"./make/lzio.d" -MT"./make/lzio.d" -o"./make/lzio.o" "src/lzio.c" 2>> ./make/result.txt 

./make/print.o : src/print.c
	g++ $(COMPILE_OPTIONS) -MF"./make/print.d" -MT"./make/print.d" -o"./make/print.o" "src/print.c" 2>> ./make/result.txt 


#====================================
# prepare
#====================================
prepare : 
	rm -r -f ./make;mkdir ./make

#====================================
# clean
#====================================
clean : 
	rm -r -f ./make
