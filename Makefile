CXX = clang++
CXXFLAGS += -std=c++11 -Wall -Werror
DEBUGFLAGS = -g -fsanitize=address -fno-omit-frame-pointer
RELEASEFLAGS = -O3
BUILDFLAGS = $(RELEASEFLAGS)
LDFLAGS += -L/usr/local/lib -I/usr/local/include \
			`pkg-config --libs protobuf librabbitmq libSimpleAmqpClient`\
			-ldl -lpthread -lAria -lm -lrt -lboost_system -lboost_filesystem -lboost_program_options -lismsgs\
			-Wl,--no-as-needed -Wl,--as-needed
PROTOC = protoc

LOCAL_PROTOS_PATH = ./msgs/
vpath %.proto $(LOCAL_PROTOS_PATH)

MAINTAINER = mendonca
SERVICE = robot-gateway
VERSION = 1
LOCAL_REGISTRY = git.is:5000

all: $(SERVICE) test

clean:
	rm -f *.o *.pb.cc *.pb.h $(SERVICE) test

$(SERVICE): $(SERVICE).o
	$(CXX) $^ $(LDFLAGS) $(BUILDFLAGS) -o $@

test: test.o
	$(CXX) $^ $(LDFLAGS) $(BUILDFLAGS) -o $@

.PRECIOUS: %.pb.cc
%.pb.cc: %.proto
	$(PROTOC) -I $(LOCAL_PROTOS_PATH) --cpp_out=. $<

docker:
	rm -rf libs/
	mkdir libs/
	lddcp $(SERVICE) libs/
	docker build -t $(MAINTAINER)/$(SERVICE):$(VERSION) .
	rm -rf libs/

push_local: docker
	docker tag $(MAINTAINER)/$(SERVICE):$(VERSION) $(LOCAL_REGISTRY)/$(SERVICE):$(VERSION)
	docker push $(LOCAL_REGISTRY)/$(SERVICE):$(VERSION)

push_cloud: docker
	docker push $(MAINTAINER)/$(SERVICE):$(VERSION)