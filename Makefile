CXX = /usr/bin/arm-linux-gnueabihf-g++
CXXFLAGS += -std=c++11 -Wall
LDFLAGS += -L/usr/arm-linux-gnueabihf/lib -I/usr/arm-linux-gnueabihf/include -pthread -lpthread \
	-lprotobuf -lismsgs -lrabbitmq -lSimpleAmqpClient -lcurl \
	-lboost_system -lboost_program_options -lboost_chrono -lboost_filesystem \
	-lAria -lm -lrt \
	-lopentracing -lzipkin -lzipkin_opentracing
PROTOC = protoc

LOCAL_PROTOS_PATH = ./msgs/
vpath %.proto $(LOCAL_PROTOS_PATH)

MAINTAINER = viros
SERVICE = robot-gateway
TEST = test
VERSION = 1
LOCAL_REGISTRY = ninja.local:5000

all: debug

debug: CXXFLAGS += -g
debug: LDFLAGS += -fsanitize=address -fno-omit-frame-pointer
debug: $(SERVICE) $(TEST)

release: CXXFLAGS += -Werror -O2
release: $(SERVICE)

clean:
	rm -f *.o *.pb.cc *.pb.h $(SERVICE) $(TEST)

$(SERVICE): $(SERVICE).o
	$(CXX)  $^ $(LDFLAGS) $(BUILDFLAGS) -o $@

$(TEST): $(TEST).o
	$(CXX) $^ $(LDFLAGS) $(BUILDFLAGS) -o $@

.PRECIOUS: %.pb.cc
%.pb.cc: %.proto
	$(PROTOC) -I $(LOCAL_PROTOS_PATH) --cpp_out=. $<

docker:
	docker build -t $(MAINTAINER)/$(SERVICE):$(VERSION) -f Dockerfile.arm --build-arg=SERVICE=$(SERVICE) .

push_local: docker
	docker tag $(MAINTAINER)/$(SERVICE):$(VERSION) $(LOCAL_REGISTRY)/$(SERVICE):$(VERSION)
	docker push $(LOCAL_REGISTRY)/$(SERVICE):$(VERSION)

push_cloud: docker
	docker push $(MAINTAINER)/$(SERVICE):$(VERSION)
