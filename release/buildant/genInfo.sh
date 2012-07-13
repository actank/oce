#!/bin/bash

ROOT=$1

cd $ROOT
svn info | grep -E "URL|Revision"

echo "Current user: $USER"
echo "Current host: $HOSTNAME"
