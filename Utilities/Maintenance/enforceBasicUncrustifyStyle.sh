#!bin/bash

# \Author hans Johnson

#find  -name "*.cxx" -exec uncrustify -c ~/Dashboards/src/ITK/Utilities/Maintenance/uncrustify_itk.cfg  --no-backup {} \;
#find  -name "*.hxx" -exec uncrustify -c ~/Dashboards/src/ITK/Utilities/Maintenance/uncrustify_itk.cfg  --no-backup {} \;
#find  -name "*.h" -exec uncrustify -c ~/Dashboards/src/ITK/Utilities/Maintenance/uncrustify_itk.cfg  --no-backup {} \;

if [ ! -d "${1}" ]; then
   echo "Missing input directory!"
   exit -1
fi

for file_extensions in "*.cxx" "*.hxx" "*.h"; do
  for my_file in   $(find $1  -name ${file_extensions} |grep -v ThirdParty); do
    uncrustify -c ${1}/Utilities/Maintenance/uncrustify_itk.cfg --no-backup ${my_file}
  done
done
