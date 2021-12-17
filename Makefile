PDK = ../pdk
CFLAGS = -W -Wall -O -I$(PDK) -std=c99
O = config.o credits.o language.o main.o message.o mine.o sendconf.o transport.o util.o utildata.o

sbreload: $(O)
	$(CC) -o $@ $(O) -L$(PDK) -lpdk -lm
