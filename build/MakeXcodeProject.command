cd `dirname $0`
mkdir -p xcode
pushd xcode
cmake -G "Xcode" ../../
popd
