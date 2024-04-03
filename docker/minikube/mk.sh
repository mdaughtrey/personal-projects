#!/bin/bash

set -o nounset

M=minikube
MK="minikube kubectl"



validate_params()
{
    if (( ${EXPECT} != $(echo $PARAMS | wc -w) )); then
        echo ${ERRMSG} "[${PARAMS}]"
        exit 1
    fi
}

# Create new namespace
do_newnamespace()
{
    EXPECT=1 ERRMSG="Missing namespace" validate_params
#    if (( 1 != $(echo $PARAMS | wc -w) )); then
#        echo missing namespace
#        exit 1
#    fi
    
    read -a NS $PARAMS
    cat <<EOF > /tmp/mk.yaml
---
apiVersion: v1
kind: Namespace
metadata:
  name: $NS
EOF

  ${MK} -- apply -f /tmp/mk.yaml

}

# Delete namespace
do_delnamespace()
{
    EXPECT=1 ERRMSG="Missing namespace" validate_params
#    if (( 1 != $(echo $PARAMS | wc -w) )); then
#        echo missing namespace
#        exit 1
#    fi
    
    read NS <<<$PARAMS
    cat <<EOF > /tmp/mk.yaml
---
apiVersion: v1
kind: Namespace
metadata:
  name: $NS
EOF

  ${MK} -- delete -f /tmp/mk.yaml

}

do_deployment()
{
    EXPECT=1 ERRMSG="Missing Deployment" validate_params
##    if (( 1 != $(echo $PARAMS | wc -w) )); then
#        echo missing deployment
#        exit 1
#    fi
    
    read  YAML <<< $PARAMS
	${MK} -- apply -f ${YAML}

}

getpodinfo()
{
    PODNAME=$1
    PODINFO=$(${MK} -- get pods -A | grep ${PODNAME})
	if [[ ! -n "${PODINFO}" ]]; then
		echo No information for pod [${PODNAME}]
		exit 1
	fi
    echo ${PODINFO}
}

do_describe()
{
    EXPECT=1 ERRMSG="Missing podname" validate_params
    read PODNAME <<< "$PARAMS"
	read NAMESPACE NAME READY STATUS RESTARTS AGE <<< $(getpodinfo ${PODNAME})
    ${MK} -- describe pod ${NAME} -n ${NAMESPACE}
}

# Run busybox shell 
do_bbshell()
{
    EXPECT=1 ERRMSG="Need podname" validate_params
    read PODNAME <<< "$PARAMS"
    read NAMESPACE NAME READY STATUS RESTARTS AGE <<< $(getpodinfo ${PODNAME})
    ${MK} -- exec -it ${NAME} -- /bin/sh
}

do_logs()
{
    EXPECT=1 ERRMSG="Need podname" validate_params
    read PODNAME <<< "${PARAMS}"
	read NAMESPACE NAME READY STATUS RESTARTS AGE <<< $(getpodinfo ${PODNAME})
    ${MK} -- logs ${NAME} -n ${NAMESPACE}
}


case "$1" in
    start) ${M} start ;;
    stop) ${M} stop ;;
    shell) shift; PARAMS=$@; do_bbshell ;;
    delete) ${M} delete ;;
	describe) shift; PARAMS=$@; do_describe ;;
    cinfo) ${MK} cluster-info ;;
    logs) shift; PARAMS=$@; do_logs ;;
    nodes) ${MK} get nodes ;;
    names) ${MK} get namespaces ;;
    pods) ${MK} -- get pods -A -o wide ;;
    services) ${MK} -- get services -A ;;
    newns) shift; PARAMS=$@ do_newnamespace ;;
    delns) shift; PARAMS=$@ do_delnamespace ;;
	newdeploy) PARAMS=$2 do_deployment ;;
	deploys) ${MK} -- get deployments -A ;;
	
    *) echo what?; exit 1 ;;
esac

