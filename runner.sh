#!/usr/bin/zsh

make -q clean && make -jq
schedule_variants=( "dynamic" "static" "guided")
chunk_size=(1 2 4 8 16 32 64)

for schedule in ${schedule_variants[@]}; do
    for chunk in ${chunk_size[@]}; do
        export OMP_SCHEDULE="$schedule,$chunk"
        for i in {-1..4}; do
            sum=0
            for run in {1..10}; do
                runtime=$(./omp4 $i test_data/in.pgm out.pgm | head -n 1 | \
                    cut -d ' ' -f 4- | cut -d ' ' -f 1)
                sum=$(bc -q <(echo "$runtime + $sum\n quit"))
            done
            echo "$schedule,$chunk" $i $sum
        done
    done 
done
