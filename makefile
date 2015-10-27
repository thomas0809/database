all:test
test:main.cpp recordmanager/rm_record.cpp recordmanager/rm_filehandle.cpp
	g++ -o test main.cpp recordmanager/rm_record.cpp recordmanager/rm_filehandle.cpp
clean:
	del test