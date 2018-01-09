CXXFLAGS += -std=c++11 -Wall -Werror -O2
LDFLAGS+= -L/usr/arm-linux-gnueabihf/lib -I/usr/arm-linux-gnueabihf/include \
			-lprotobuf -lrabbitmq -lSimpleAmqpClient \
			-lboost_system -lboost_filesystem -lboost_program_options -lboost_chrono \
			-ldl -lpthread -lAria -lm -lrt -lismsgs \
			-lprometheus-cpp  -lopentracing -lzipkin -lzipkin_opentracing -lcurl
PROTOC = protoc

CROSS_IMAGE = git.is:5000/is-cpp:1-aria
SERVICE = robot-gateway
RPI_IP = 10.61.0.26

LOCAL_PROTOS_PATH = ./msgs/
vpath %.proto $(LOCAL_PROTOS_PATH)

all: cross

clean:
	rm -f *.o *.pb.cc *.pb.h $(SERVICE) test
	rm -rf deploy

cross:
	docker run -ti -v `pwd`:/opt -w /opt $(CROSS_IMAGE) make build

build: $(SERVICE) test
	rm -rf deploy/*
	mkdir -p deploy
	./arm-ldd $(SERVICE) deploy/
	mv $(SERVICE) deploy/
	mv test deploy/

# run: cross
# 	docker run -ti --network=host -v `pwd`:/opt -w /opt $(CROSS_IMAGE) qemu-arm deploy/$(SERVICE) $(args)

deploy: cross
	scp -r deploy pi@$(RPI_IP):gw

$(SERVICE): $(SERVICE).o
	$(CXX) $^ $(LDFLAGS) -o $@

test: test.o
	$(CXX) $^ $(LDFLAGS) -o $@

.PRECIOUS: %.pb.cc
%.pb.cc: %.proto
	$(PROTOC) -I $(LOCAL_PROTOS_PATH) --cpp_out=. $<