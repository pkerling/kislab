template<typename T>
void debugprintln(T const& t)
{
#ifdef DEBUG
  Serial.println(t);
#endif
}

template<typename T>
void debugprint(T const& t)
{
#ifdef DEBUG
  Serial.print(t);
#endif
}
