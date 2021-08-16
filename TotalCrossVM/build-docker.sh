#!/bin/bash

dockerBuildCommonStep()
{
   export DOCKER_CLI_EXPERIMENTAL=enabled
   export DOCKER_BUILDKIT=1
   docker run --rm --privileged multiarch/qemu-user-static --reset -p yes 
   docker buildx create --use --name armbuilder
   docker buildx inspect --bootstrap
}

helpFunction()
{
   echo ""
   echo "Usage: $0 -p platform"
   echo -e "\t-p You should pass the platform."
   echo -e "\t-p platforms accepted are amd64,arm32v7 and arm64"
   exit 1 # Exit script after printing help
}

while getopts "p:" opt
do
   case "$opt" in
      p ) platform="$OPTARG" ;;
      ? ) helpFunction ;; # Print helpFunction in case parameter is non-existent
   esac
done

# Print helpFunction in case parameter is empty
if [ -z "$platform" ]
then
   echo "platform is empty !";
   helpFunction
fi

# Begin script in case all parameters are correct
case "$platform" in 
   
   arm32v7) 
      dockerBuildCommonStep
      docker buildx build --platform linux/arm/v7 --load -t totalcross/linux-arm32v7-build -f docker/arm32v7/Dockerfile .
   ;;
   
   amd64)
      dockerBuildCommonStep
      docker buildx build --platform linux/amd64 --load -t totalcross/linux-amd64-build -f docker/amd64/Dockerfile .
   ;;
   
   arm64)
      dockerBuildCommonStep
      docker buildx build --platform linux/arm64 --load -t totalcross/linux-arm64-build -f docker/arm64/Dockerfile .
   ;;
   
   *) 
      echo " invalid option !" 
   ;;
esac
