
CFLAGS=-g  -Wall -I../ 
LIBS= -lphidget21 -lpthread -ldl -lrt -lm -lusb


control_servo_diana:control_servo_diana.o 
	$(CC) $(CFLAGS) control_servo_diana.c -o control_servo_diana  $(LIBS)

