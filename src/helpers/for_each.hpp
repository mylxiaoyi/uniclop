

#if !defined(FOR_EACH_HEADER)
#define FOR_EACH_HEADER

#define for_each(the_iterator, the_list)\
for (the_iterator = the_list.begin(); the_iterator != the_list.end(); ++the_iterator)\


#endif // FOR_EACH_HEADER