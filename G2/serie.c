int fact(int n) {
   int fat = 1;

   for(int i = 1;i<=n; i++){
	fat = fat * i;	
   }
   return fat;  
}


double pot(double x, int n) {
  double expo = 1;

  for(int i = 0; i<n; i++){
     expo = expo * x;
  }	
  return expo;
}



/**
 * Insere esta fórmula no https://www.quicklatex.com/
 * \sum_{n=0}^{\infty} \frac{(2n)!}{4^n(n!)^2(2n+1)} x^{2n+1} 
 */
double serie(double x, int n) { return 0; }
