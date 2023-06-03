.PHONY: all clean

all: st_pipeline

st_pipeline: st_pipeline.c
	gcc -o st_pipeline st_pipeline.c -lm -pthread

clean:
	rm -f st_pipeline