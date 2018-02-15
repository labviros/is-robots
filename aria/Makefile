MAINTAINER = viros
VERSION = 1-aria
LOCAL_REGISTRY = ninja.local:5000

all: docker

docker:
	docker build -t $(MAINTAINER)/is-cpp:$(VERSION) .

push_local: docker
	docker tag $(MAINTAINER)/is-cpp:$(VERSION) $(LOCAL_REGISTRY)/is-cpp:$(VERSION)
	docker push $(LOCAL_REGISTRY)/is-cpp:$(VERSION)

push_cloud: docker
	docker push $(MAINTAINER)/is-cpp:$(VERSION)
