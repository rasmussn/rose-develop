
struct seq_exec{};

template<typename POLICY, typename IdxI=int, typename TI, typename BODY>
void forallN(TI const &is_i, BODY const &body){}

void foobar()
   {
     int is; 
     forallN<seq_exec>(is, [=] (int i) { 42; });
   }
