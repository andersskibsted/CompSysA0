#!/usr/bin/env bash

# Exit immediately if any command below fails.
set -e

make


echo "Generating a test_files directory.."
mkdir -p test_files
rm -f test_files/*

generate_iso_byte() {
    if (( RANDOM % 2 )); then
        # Range 0-127
        echo $((RANDOM % 121 + 7))
    else
        # Range 160-255
        echo $((RANDOM % 96 + 160))
    fi
}
generate_ascii_byte() {
    echo $((RANDOM % 128))
}


# Generer 20 bytes



echo "Generating test files.."
### ASCII test files
printf "Hello, World!\n" > test_files/ascii.input
printf "Hello, World!" > test_files/ascii2.input
printf "       " > test_files/spaces.input
for i in {1..100}; do printf "%02x " $(((RANDOM % 127)+1)); done > test_files/random_ascii.input
for i in {1..10}; do printf "%02x " $(((RANDOM % 127)+1)); done > test_files/random_ascii2.input
cat python_notebook.ipynb > test_files/python_notebook.input
### Data test files
printf "Hello,\x00\x00\x00\x00 World!\n" > test_files/data.input
printf "\0%.0s" {1..100} > test_files/all_zeros.input
head -c 100 /dev/urandom > test_files/random_bytes.input
head -c 200 /dev/urandom > test_files/random_bytes2.input
head -c 10 /dev/urandom > test_files/random_bytes3.input
head -c 1 /dev/urandom > test_files/one_random_byte.input
cat file > test_files/binary_executable.input
### UTF-8 test files
printf "Hej med æøå og emojis" > test_files/utf8.input
printf "😠😠😠😠😠😠😠 <->_=^" > test_files/utf82.input
printf "한자" > test_files/utf8korean.input
printf "漢字" > test_files/utf8japanese.input
printf "𡨸漢" > test_files/utf8vietnamese.input
printf "𡨸𡨸" > test_files/utf8vietnameserepeat1.input
printf "漢漢" > test_files/utf8vietnameserepeat2.input
printf "漢字" > test_files/utf8tradchinese.input
printf "汉字" > test_files/utf8simplchinese.input
printf "ἀπόστροφος" > test_files/utf8greek.input
printf "	в	г	д	—	е	ж	з	и	й	—	к	л	—	м	н	—	о	п	р	с	" > test_files/utf8cyrillic.input
printf "𓁀	𓁁	𓁂	𓁃	𓁄	𓁅	𓁆	𓁇	𓁈	𓁉	𓁊	𓁋	𓁌	𓁍	𓁎" > test_files/utf8hieroglyphs.input

### ISO test files
printf "æøå45eeaoy" | iconv -f UTF-8 -t ISO-8859-1 > test_files/iso.input
printf "æøåæøåæøåæøåæøåæøåæøåæøåæøåæøåæøåæøåæøåæøåæøåæøåæøåæøå" | iconv -f UTF-8 -t ISO-8859-1 > test_files/iso2.input

# 100 max value iso bytes
for i in {0..99}; do printf "\xff"; done > test_files/iso_100_FF.input

# 100 random iso bytes in upper part of iso range
iso_bytes_high_range=()
for i in {1..100}; do
    iso_bytes_high_range+=($((160 + (RANDOM % 96))))
done

for byte in "${iso_bytes_high_range[@]}"; do
    printf "\\x$(printf "%02x" $byte)" >> test_files/random_iso_only_higher_values.input
done
#  20 random iso bytes
iso_bytes_20=()
for ((i=0; i<20; i++)); do
    iso_bytes_20+=($(generate_iso_byte))
done

for byte in "${iso_bytes_20[@]}"; do
    printf "\\x$(printf "%02x" $byte)" >> test_files/random_iso2.input
done
# 100 random iso bytes
iso_bytes_100=()
for ((i=0; i<100; i++)); do
    iso_bytes_100+=($(generate_iso_byte))
done

for byte in "${iso_bytes_100[@]}"; do
    printf "\\x$(printf "%02x" $byte)" >> test_files/random_iso3.input
done



### Permission denied
printf "" > test_files/secret.input
chmod -r test_files/secret.input
### Empty
printf "" > test_files/empty.input
### TODO: Generate more test files ###


echo "Running the tests.."
exitcode=0
for f in test_files/*.input
do
  echo ">>> Testing ${f}.."
  file    ${f} | sed -e 's/ASCII text.*/ASCII text/' \
                         -e 's/UTF-8 Unicode text.*/UTF-8 Unicode text/' \
                         -e 's/ISO-8859 text.*/ISO-8859 text/' \
                         -e 's/writable, regular file, no read permission/cannot determine (Permission denied)/' \
                         > "${f}.expected"
  ./file  "${f}" > "${f}.actual"

  if ! diff -u "${f}.expected" "${f}.actual"
  then
    echo ">>> Failed :-("
    exitcode=1
  else
    echo ">>> Success :-)"
  fi
done

echo "Testing non-existent file"
file non-existent.input > ./test_files/non-existent.expected
./file non-existent.input > ./test_files/non-existent.actual

if ! diff -u non-existent.expected non-existent.actual
then
  echo ">>> Failed :-("
  exitcode=1
else
  echo ">>> Success :-)"
fi

exit $exitcode
