for f in census1881_srt  census-income_srt census1881      census-income weather_sept_85_srt  wikileaks-noquotes_srt  wikileaks-noquotes; do
  echo 
  echo
  ./roaring_benchmarks -r CRoaring/benchmarks/realdata/$f;
done
