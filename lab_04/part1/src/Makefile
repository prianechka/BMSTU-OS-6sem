CC := g++

OBJS := app.o

app.exe : $(OBJS)
	$(CC) $^ -o $@ -pthread

%.o : %.cpp
	$(CC) -c $<

.PHONY : clean

clean :
	rm *.o *.exe
