OUTPUT=$1

echo "#define GIT_REVISION_ID \""$(git rev-parse HEAD)"\"" > $OUTPUT
