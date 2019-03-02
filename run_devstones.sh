#!/bin/zsh
PREFIX=generated_`date +%Y%m%d`
EXTERNAL=100
INTERNAL=100
EVENTS=events.txt

mkdir -p ${PREFIX}
cp events.txt ${PREFIX}/events.txt

# Here we generated models for Cadmium
for D in `seq 2 1 10`; do
	for W in `seq 2 1 10`; do
		echo "Generating model W:${W} D:${D}"
 		./cadmium-devstone   \
                            --kind=LI    \
                            --width=${W} \
                            --depth=${D} \
                            --ext-cycles=${EXTERNAL} \
                            --int-cycles=${INTERNAL} \
                            --event-list=${EVENTS}\
                            --output="${PREFIX}/LI_DEVSTONE_D${D}_W${W}.cpp"
        done
done

# Here we build the generated models and log their metrics
for D in `seq 2 1 10`; do
	for W in `seq 2 1 10`; do
		echo "Building model W:${W} D:${D}"
                /usr/bin/time -f "%e" \
                clang++ --std=c++17 -ftemplate-depth=2048 -Isimulators/cadmium/include -Isrc \
                        "${PREFIX}/LI_DEVSTONE_D${D}_W${W}.cpp" dhry/dhry_1.o dhry/dhry_2.o \
                        -o "${PREFIX}/LI_DEVSTONE_D${D}_W${W}"
        done
done
# Here we run the generated models and log their executions
for D in `seq 2 1 10`; do
        for W in `seq 2 1 10`; do
                echo "Running model W:${W} D:${D}"
                /usr/bin/time -f "%e" \
                "${PREFIX}/LI_DEVSTONE_D${D}_W${W}"
        done
done
