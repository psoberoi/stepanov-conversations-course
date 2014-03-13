comment: Thanks to Paul McJones for porting the code to Algol 60 ;

begin

    procedure unguardedlinearinsert(A, last, val);
        value last, val; integer last, val;
        integer array A;
    begin
        integer prev;
        prev := last - 1;
        while val < A[prev] do
            begin
                A[last] := A[prev];
                last := prev;
                prev := prev - 1
            end;
        A[last] := val
    end;

    procedure unguardedinsertionsort(A, first, last);
        value first, last; integer first, last;
        integer array A;
    begin
        integer i;
        for i := first step 1 until last - 1 do
            unguardedlinearinsert(A, i, A[i])
    end;


    procedure linearinsert(A, first, last, val);
        value first, last, val; integer first, last, val;
        integer array A;
    begin
        if val < A[first] then
            begin
                while first != last do
                    begin
                        A[last] := A[last - 1];
                        last := last - 1
                    end;
                A[first] := val
            end
        else unguardedlinearinsert(A, last, val)
    end;

    procedure insertionsort(A, first, last);
        value first, last; integer first, last;
        integer array A;
    begin
        integer i;
        if first != last then
            for i := first + 1 step 1 until last - 1 do
                linearinsert(A, first, i, A[i]);
    end;

    integer procedure medianof3(a, b, c);
        value a, b, c; integer a, b, c;
        if a < b then
            begin
                if b < c then medianof3 := b
                else
                    if a < c then medianof3 := c
                    else medianof3 := a
            end
        else
            if a < c then medianof3 := a
            else
                if b < c then medianof3 := c
                else medianof3 := b;

    procedure exchange (A, B); integer A, B;
    begin integer T; T := A; A := B; B := T end;

    integer procedure unguardedpartition(A, first, last, pivot);
        value first, last, pivot; integer first, last, pivot;
        integer array A;
    begin
        integer tmp;
        last := last - 1;
        while A[first] < pivot do first := first + 1;
        while pivot < A[last] do last := last - 1;
        while first < last do
            begin
                comment exchange(A[first], A[last]);
                tmp := A[first];
                A[first] := A[last];
                A[last] := tmp;
                first := first + 1; last := last - 1;
                while A[first] < pivot do first := first + 1;
                while pivot < A[last] do last := last - 1;
            end;
        unguardedpartition := first;
    end;

    procedure quicksortloop(A, first, last, threshold);
        value first, last, threshold; integer first, last, threshold;
        integer array A;
    begin
        integer len;
        integer middle;
        integer pivot;
        integer cut;
        len := last - first;
        while len > threshold do
            begin
                middle := first + (last - first) div 2;
                pivot := medianof3(A[first], A[middle], A[last - 1]);
                cut := unguardedpartition(A, first, last, pivot);
                if last - cut < cut - first then
                    begin
                        quicksortloop(A, cut, last, threshold);
                        last := cut;
                    end
                else
                    begin
                        quicksortloop(A, first, cut, threshold);
                        first := cut;
                    end;
                len := last - first;
            end;
    end;

    procedure quicksort(A, first, last);
        value first, last; integer first, last;
        integer array A;
    begin
        integer len;
        integer threshold;
        integer middle;
        len := last - first;
        threshold := 16;
        if len <= threshold then
            insertionsort(A, first, last)
        else
            begin
                quicksortloop(A, first, last, threshold);
                middle := first + threshold;
                insertionsort(A, first, middle);
                unguardedinsertionsort(A, middle, last)
            end
    end;

    real procedure rand; rand := EXT(41);

    integer procedure random(M, N); value M, N; integer M, N;
    random := M + entier(rand * (N - M + 1));

    procedure randomshuffle(A, first, last);
        value first, last; integer first, last;
        integer array A;
    begin
        integer i;
        integer randomPlace;
        integer tmp;
        i := first + 1;
        while i < last do begin
            randomPlace := random(0, i - first + 1);
            exchange(A[randomPlace], A[i]);
            i := i + 1
        end
    end;

    procedure iota(A, n);
        value n; integer n;
        integer array A;
    begin
        integer i;
        for i := 0 step 1 until n - 1 do A[i] := i
    end;

    procedure arraycopy(source, sourcefirst, dest, destfirst, size);
        value sourcefirst, destfirst, size; integer sourcefirst, destfirst, size;
        array source, dest;
    begin
        integer i;
        for i := 0 step 1 until size - 1 do
            dest[destfirst + i] := source[sourcefirst + i]
    end;

    procedure sortedarray(A, size);
        value size; integer size;
        array A;
    begin
        integer i;
        if size < 2 then sortedarray := true
        else
            begin
                i := 0;
                while i < size - 2 and A[i] < A[i + 1] do i := i + 1;
                sortedarray := i = size - 2;
                if i < size - 2 then
                    begin
                        PRINTTEXT(`OUT OF ORDER: '); print(i); print(A[i]); print(i + 1); print(A[i + 1]); NLCR
                    end
            end
    end;

    real procedure clock; clock := EXT(11);

    real procedure timeSort(data, length, buffer, size);
        value length, size; integer length, size;
        integer array data; integer array buffer;
    begin
        integer first;
        real startTime;
        first := 0;
        startTime := clock;
        while first <= length - size do
            begin
                arraycopy(data, first, buffer, 0, size);
                quicksort(buffer, 0, size);
                if not sortedarray(buffer, size) then
                    begin
                        integer i;
                        PRINTTEXT(`out of order');
                        for i := 0 step 1 until size do print(buffer[i]);
                        EXT(71) /* panic */
                    end;
                first := first + size;
            end;
        timeSort := clock - startTime;
    end;

    integer minsize;
    integer maxsize;
    minsize := 8;
    maxsize := 16 * 1024 * 1024;
    begin
        integer array data[0 : maxsize - 1];
        integer array buffer[0 : maxsize - 1];
        integer arraysize;
        real time;

        PRINTTEXT (`size     time'); NLCR;

        iota(data, maxsize);
        randomshuffle(data, 0, maxsize);

        arraysize := minsize;
        while arraysize <= maxsize do
            begin
                time := timeSort(data, maxsize, buffer, arraysize);
                print(arraysize); print(time); NLCR;
                arraysize := 2 * arraysize;
            end
    end
end.
