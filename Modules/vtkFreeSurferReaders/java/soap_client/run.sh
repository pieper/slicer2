#!/bin/sh

unset CLASSPATH
if [ "$CP" == "" ]
then
  CP=/home/ajoyner/apps/slicer2.2-dev-linux-x86-2003-09-05/Modules/vtkFreeSurferReaders/java/soap_client
fi
if [ ! -d "$CP" ]
then
  echo ""
  echo "Cannot continue"
  echo "Please set the environment var CP to a valid directory:$CP"
  exit 1;
fi

for lib in $CP/lib/*.jar
do
  if [ "$CLASSPATH" == "" ]
  then
    CLASSPATH=$lib
  else
    CLASSPATH=${CLASSPATH}:$lib
  fi
done
CLASSPATH=$CLASSPATH:$CP/classes

java -cp $CLASSPATH  StatsSOAPClient -p8080 -h192.168.147.71 $* > $CP/results.txt
#java -cp $CLASSPATH  StatsSOAPClient -p8080 -h192.168.147.71 $*
