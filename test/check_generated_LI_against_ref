#!/bin/zsh
THIS_PATH=${0:a:h}
PREFIX=${THIS_PATH}/TEST_ARENA
W=3
D=3
EXTERNAL=100
INTERNAL=100
EVENTS=events.txt

mkdir -p ${PREFIX}
# Here we generated models for Cadmium
echo "Generating model W:${W} D:${D}"
./cadmium-devstone   \
    --kind=LI    \
    --width=${W} \
    --depth=${D} \
    --ext-cycles=${EXTERNAL} \
    --int-cycles=${INTERNAL} \
    --event-list=${EVENTS}\
    --output="${PREFIX}/LI_DEVSTONE_D${D}_W${W}.cpp"

# Diff between the 2 files ignoring spaces, tabs, blank lines and comments
diff -b -w -E -B -I '//.*' ${THIS_PATH}/../src/cadmium-ref-LI.cpp "${PREFIX}/LI_DEVSTONE_D${D}_W${W}.cpp"
exit $?
