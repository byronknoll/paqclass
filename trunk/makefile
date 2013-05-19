all : PAQclass runner

runner : runner.cpp
	g++ runner.cpp -O2 -o runner

PAQclass : PAQclass.cpp
	g++ PAQclass.cpp -DUNIX -O2 -DNOASM -s -o PAQclass

clean:
	rm -f PAQclass runner temp.PAQclass
