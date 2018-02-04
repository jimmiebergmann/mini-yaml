test: folders ../obj/test/test.o ../obj/test/Yaml.o
	g++ -o ../bin/test ../obj/test/test.o ../obj/test/Yaml.o  -s  googletest/googletest/make/gtest_main.a -lpthread

../obj/test/test.o: test.cpp
	cc -std=c++11 -Igoogletest/googletest/include -I../yaml -c test.cpp -o ../obj/test/test.o

../obj/test/Yaml.o: ../yaml/Yaml.cpp
	cc -std=c++11 -I../yaml -c ../yaml/Yaml.cpp -o ../obj/test/Yaml.o

folders:
	mkdir -p ../obj/test
	

.PHONY: clean
clean:
	rm -r ../obj
