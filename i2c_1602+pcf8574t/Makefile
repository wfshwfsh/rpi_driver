CROSS_COMPILE:=arm-linux-gnueabihf-

#TAR = i2c_test
TAR = lightSensor

CC = gcc
CFLAG = -Wall

all: clean $(TAR) exec

$(TAR): 	
	$(CC) $(TAR).c -o $(TAR).app $(CFLAG)

clean:
	rm -f $(TAR).app

exec: 
	./$(TAR).app
