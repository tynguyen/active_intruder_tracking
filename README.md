![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
# Active Intruder Tracking

# Structure
```
|_docker: scripts to create, launch, attach containers
	|_my_docker_env.sh: set environmental variables including the name of the Docker Image, container's name ... Need to source this file at the beginning
	|_create_container.sh: create a container if not existed yet "bash create_container.sh -v <folder to share with the container>"
	|_attach_container.sh: (launch if not yet), log into a running container
	|_rm_container.sh: (stop if not yet), remove a container
	|_stop_container.sh: stop a running container
|_src: source files
|_assets: media files used to write README
```

# Requirements
- [x] locate
```
sudo apt install mlocate
```

- [x] eigen3
```
sudo apt install libeigen3-dev
```

Check if it's already installed
```
sudo locate eigen3
```

- [x] git version >= 2.7
```
sudo apt-get install python-software-properties software-properties-common
sudo add-apt-repository ppa:git-core/ppa -y
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install git -y
```
