cd (Split-Path -Parent $MyInvocation.MyCommand.Definition)

phpize

./configure

cmake .

make -j
