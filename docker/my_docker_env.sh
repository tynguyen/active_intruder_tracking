# Base docker image that you want to build upon on
export BASE_DOCKER_IMG_NAME="tynguyen/saicny_ubuntu18.04"
export BASE_DOCKER_IMG_TAG="ros-melodic-cuda10.2-cudnn8"

: ' Name the docker image that you want to build i.e: $DOCKER_IMG_NAME:$DOCKER_IMG_TAG
If you do not change anything, by default, your docker image name will be
tynguyen/saicny_ubuntu18.04:<your user name>_cuda10.2-cudnn8
'
user_name=$(whoami)

export DOCKER_IMG_NAME=$BASE_DOCKER_IMG_NAME
export DOCKER_IMG_TAG="${user_name}-${BASE_DOCKER_IMG_TAG}"
export CONTAINER_NAME="${user_name}-${BASE_DOCKER_IMG_TAG}"
