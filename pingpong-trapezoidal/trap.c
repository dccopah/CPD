#include <stdio.h>

double f(double x);    //Función que se va integrar 
double Trap(double a, double b, int n, double h);

int main(void) {
   double  integral;   // Almacena el resultado en integral
   double  a, b;       // Left y right puntos finales
   int     n;          // Número de trapezoides
   double  h;          // Altura de trapezoides

   printf("Enter a, b, and n\n");
   scanf("%lf", &a);
   scanf("%lf", &b);
   scanf("%d", &n);

   h = (b-a)/n;
   integral = Trap(a, b, n, h);
   
   printf("Con n = %d trapezoides, nuestra estimación\n", n);
   printf("de la integral de %f a  %f = %.15f\n",
      a, b, integral);

   return 0;
}  
// Estima la integral de "a" a "b" de la funcion f usando la regla n trapezoides
double Trap(double a, double b, int n, double h) {
   double integral;
   int k;
   integral = (f(a) + f(b))/2.0;
   for (k = 1; k <= n-1; k++) {
     integral += f(a+k*h);
   }
   integral = integral*h;
   return integral;
}  
// Calcula el valor de la función a integrar
double f(double x) {
   double return_val;

   return_val = x*x;
   return return_val;
}  
