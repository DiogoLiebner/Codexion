tmp=$(mktemp)
trap 'rm -f "$tmp"' EXIT

for scheduler in fifo edf; do
    echo "===== Testing $scheduler ====="

    for run in $(seq 1 50); do
        printf "Run %2d/50 ... " "$run"

        ./codexion 20 2000 200 200 200 20 100 "$scheduler" > "$tmp" 2>&1

        failed=0

        for coder in $(seq 1 20); do
            count=$(grep -cFx "coder $coder has compiled 20 times" "$tmp")

            if [ "$count" -ne 1 ]; then
                failed=1
                break
            fi
        done

        if [ "$failed" -eq 1 ]; then
            echo "FAIL"
            echo
            echo "Failed on $scheduler, run $run"
            cat "$tmp"
            exit 1
        fi

        echo "OK"
    done

    echo
done

echo "All 100 runs passed (50 FIFO + 50 EDF)."