#!/usr/bin/env bash

# Runs a docker container with the image created by build_img.sh
# Requires:
#   docker
#   nvidia-docker (optional)
#   an X server (optional)
# Based on your setting, you can remove arguments listed belows.

HELP()
{
  # Display help
  echo "*Prerequisites: run "
  echo "         source my_docker_env.sh"
  echo "*Syntax: "
  echo "         bash $(basename $0) [Options]"
  echo "*Options:" 
  echo "         -h: display Help"
  echo "         -i <docker image with tag>: docker image name i.e: docker:latest. This will overwrite DOCKER_IMG_NAME and DOCKER_IMG_TAG"
  echo "         -v <absolute path to the directory on the host machine>: share this path with the docker container "  
  echo "         -n <name>: name of the docker container you want to creater"  
  echo "         -u <user name>: user name in the docker container. Shared directory will be stored to /home/<user name>/"
  echo "         -p <port mapping>: i.e: 0.0.0.0:7008: 7008" 

}

# Default user name
DOCKER_USER=$(whoami)

# Default port mapping 
PORT_OPTS=0.0.0.0:7008:7008

# Default docker image name given from $DOCKER_IMG_NAME
IMG=$DOCKER_IMG_NAME:$DOCKER_IMG_TAG


# Parse arguments
# "i:", "v:", "n:" mean that these arg needs an additional argument
# "h" means that no need an additional argument
while getopts "hi:v:n:p:u" flag
do
  case "${flag}"  in 
    h) # display Help
      HELP 
      exit;;
    i) IMG=${OPTARG};; 
    v) WS_DIR=${OPTARG} 
      WS_DIRNAME=$(basename $WS_DIR)
      echo ">> Sharing Workspace: $WS_DIR"
      DOCKER_OPTS="$DOCKER_OPTS -v $WS_DIR:/home/$DOCKER_USER/$WS_DIRNAME:rw";;
    n) CONTAINER_NAME=${OPTARG};; 
    u) DOCKER_USER=${OPTARG};; 
    p) PORT_OPTS=${OPTARG};; 
    \?) # incorrect option
      echo "Error: Invalid option."
      echo "run bash $(basename $0) -h to learn more."
      exit;;
    esac
done 



# Make sure processes in the container can connect to the x server
# Necessary so gazebo can create a context for OpenGL rendering (even headless)
XAUTH=/tmp/.docker.xauth
if [ ! -f $XAUTH ]
then
    xauth_list=$(xauth nlist :0 | sed -e 's/^..../ffff/')
    if [ ! -z "$xauth_list" ]
    then
        echo $xauth_list | xauth -f $XAUTH nmerge -
    else
        touch $XAUTH
    fi
    chmod a+r $XAUTH
fi

# Share your vim settings.
#VIMRC=$HOME/.vim
#DOCKER_OPTS="$DOCKER_OPTS -v $VIMRC:/home/$DOCKER_USER/.vim:rw"

# Mount extra volumes if needed.
# E.g.:
# -v "/opt/sublime_text:/opt/sublime_text" \

#--rm will remove the container after exitting

# Arguments to enable audio on the container
DOCKER_OPTS="$DOCKER_OPTS  --group-add $(getent group audio | cut -d: -f3)"
DOCKER_OPTS="$DOCKER_OPTS  -e PULSE_SERVER=unix:${XDG_RUNTIME_DIR}/pulse/native"
DOCKER_OPTS="$DOCKER_OPTS  -v ${XDG_RUNTIME_DIR}/pulse/native:${XDG_RUNTIME_DIR}/pulse/native"
DOCKER_OPTS="$DOCKER_OPTS  -v $HOME/.config/pulse/cookie:/root/.config/pulse/cookie"
DOCKER_OPTS="$DOCKER_OPTS  --device /dev/snd"

# Arguments to enable time sync between the docker container and the host machine
DOCKER_OPTS="$DOCKER_OPTS  -v /etc/localtime:/etc/localtime:ro" 
DOCKER_OPTS="$DOCKER_OPTS  -v /etc/timezone:/etc/timezone:ro"
  

# Arguments to enable visualization windows from the container pop up on the host machine
DOCKER_OPTS="$DOCKER_OPTS  -e DISPLAY=$DISPLAY"
DOCKER_OPTS="$DOCKER_OPTS  -e QT_X11_NO_MITSHM=1"
DOCKER_OPTS="$DOCKER_OPTS  -e XAUTHORITY=$XAUTH"
DOCKER_OPTS="$DOCKER_OPTS  -v $XAUTH:$XAUTH"
DOCKER_OPTS="$DOCKER_OPTS  -v /tmp/.X11-unix:/tmp/.X11-unix" 
DOCKER_OPTS="$DOCKER_OPTS  -e LIBGL_ALWAYS_INDIRECT=" 
DOCKER_OPTS="$DOCKER_OPTS  -e LIBGL_ALWAYS_SOFTWARE=1"


# Arguments to run GPU within the container
nvidia-docker &> /dev/null
if [ $? -ne 0 ]; then
  echo "[Warning] nvidia-docker has NOT been installed on the host machine -> NOT able to use GPUs on the container"
else
  DOCKER_OPTS="$DOCKER_OPTS  --runtime nvidia"
  echo "[Info] nvidia-docker has been installed on the host machine -> able to use GPUs on the container"
fi

echo "[Info] List of arguments given: $DOCKER_OPTS"


# In order to enable NVIDIA-driver use within a container, you need either
# - use nvidia-docker (install nvidia-docker-tool)
# - Or put a flag '--runtime nvidia' after 'docker run'
echo "----------------------------------"
echo "[Info] Creating container ${CONTAINER_NAME} from docker $IMG ...."
xhost +
docker run -it --gpus all\
  --name=$CONTAINER_NAME \
  -p $PORT_OPTS \
  -v /etc/group:/etc/group:ro \
  -v /etc/passwd:/etc/passwd:ro \
  -v "/dev/input:/dev/input" \
  --privileged \
  --security-opt seccomp=unconfined \
  $DOCKER_OPTS \
  $IMG	\
  bash


echo "----------------------------------"

