#include "lbf.hpp"

#define randint(a,b) (rand()%((b)-(a)+1)+(a))
#define randINT(tab) (tab[randint(0,2)])

using namespace std;

// Set length-dimensional vector v to zero 
void reinitialize(int* v, int length)
{
	for (int i = 0; i < length; ++i)
		v[i] = 0;
}

// print vector c in human readable form (debugging purpose)
void vectorPrint(int n, int* v)
{
	cout << "(";
	for (int i = 0; i < n-1; ++i)
		cout << v[i] << ", ";
	cout << v[n-1] << ")\n";
}

// key generation 
void keyGen(int n, int q, int q3, int q9, int* x, int* y)
{
	srand(time(NULL));
//    srand(42); // debugging purpose

	int tab[3] = {0,1,q-1};

	x[0] = (q3 + randINT(tab))%q;
	y[0] = (q9 + randINT(tab))%q;

	for(int i=1 ; i<n ; i++)
	{
		x[i] = randINT(tab);
		y[i] = randINT(tab);
	}
}

// Returns the result of H*(v1, v2) without expanding matrix H = (x | y)
void leftMultiplyByH(int n, int q, int* x, int* y, int* v1, int* v2, int* result)
{
	if (result==NULL)
	{
		cerr << "result == NULL in leftMultiplyByH\n";
		exit(-1);
	}

	reinitialize(result, n);

	/* result += x*v1 + y*v2 */ 
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			if (i<=j)  //+1
				result[i] = (result[i] + x[(n+j-i)%n]*v1[j] + y[(n+j-i)%n]*v2[j])%q;
			else  //-1
				result[i] = (result[i] + (q-x[(n+j-i)%n])*v1[j] + (q-y[(n+j-i)%n])*v2[j])%q;
		}
	}
}

// Returns vector a+b mod q
void vectorAdd(int n, int q, int* a, int* b, int* result)
{
	if (result==NULL)
	{
		cerr << "result == NULL in vectorAdd\n";
		exit(-1);
	}

	for (int i = 0; i < n; ++i)
		result[i] = (a[i]+b[i])%q;	
}

// Returns vector a-b mod q
void vectorSub(int n, int q, int* a, int* b, int* result)
{
	if (result==NULL)
	{
		cerr << "result == NULL in vectorSub\n";
		exit(-1);
	}

	for (int i = 0; i < n; ++i)
		result[i] = (a[i]+(q-b[i]))%q;	
}

// Generate random noise from {-1, 0, 1}
void noiseGen(int n, int q, int* r1, int* r2, int* e)
{
	int tab[3] = {0,1,q-1};

	for (int i = 0; i < n; ++i)
	{
		r1[i] = randINT(tab);
		r2[i] = randINT(tab);
		e[i]  = randINT(tab);
	}
}

// Compute the syndrome s = x*r1+y*r2+e
void computeSyndrome(int n, int q, int* x, int* y, int* r1, int* r2, int* e, int* result)
{
	leftMultiplyByH(n, q, x, y, r1, r2, result);
	vectorAdd(n, q, result, e, result);
}

// Test whether all the coordinates of e are in {-1, 0, 1} or not
int test(int n, int q, int* e)
{
	for (int i = 0; i < n; ++i)
		if ((e[i]!=q-1)&&(e[i]!=0)&&(e[i]!=1))
			return 0;
	return 1;
}

// Magnitude correction of r1
void setR1(int n, int q, int* r1, int* r1_tmp)
{
	int q16, q12, q56;
	double tmp, fracpart, intpart;
	tmp = ((double)q)/6;
	fracpart = modf(tmp, &intpart);
	if (fracpart <= 0.5)
		q16 = (int) floor(tmp);
	else
		q16 = (int) ceil(tmp);
	tmp = ((double)q)/2;
	fracpart = modf(tmp, &intpart);
	if (fracpart <= 0.5)
		q12 = (int) floor(tmp);
	else
		q12 = (int) ceil(tmp);
	tmp = ((double)5*q)/6;
	fracpart = modf(tmp, &intpart);
	if (fracpart <= 0.5)
		q56 = (int) floor(tmp);
	else
		q56 = (int) ceil(tmp);

	for (int i = 0; i < n; ++i)
	{
		if ((q16 < r1[i]) && (r1[i] <= q12))
			r1_tmp[i] = 1;
		else
		{
			if ((q12 < r1[i]) && (r1[i] <= q56))
				r1_tmp[i] = q-1;
			else
				r1_tmp[i] = 0;
		}
	}
}

// Magnitude correction of r2
void setR2(int n, int q, int q3, int* r2, int* r2_tmp)
{
	int q118, q16, q518;
	double tmp, fracpart, intpart;
	tmp = ((double)q)/18;
	fracpart = modf(tmp, &intpart);
	if (fracpart <= 0.5)
		q118 = (int) floor(tmp);
	else
		q118 = (int) ceil(tmp);
	tmp = ((double)q)/6;
	fracpart = modf(tmp, &intpart);
	if (fracpart <= 0.5)
		q16 = (int) floor(tmp);
	else
		q16 = (int) ceil(tmp);
	tmp = ((double)5*q)/18;
	fracpart = modf(tmp, &intpart);
	if (fracpart <= 0.5)
		q518 = (int) floor(tmp);
	else
		q518 = (int) ceil(tmp);

	for (int i = 0; i < n; ++i)
	{
		if ((q118 < (r2[i]%q3)) && ((r2[i]%q3) <= q16))
			r2_tmp[i] = 1;
		else
		{
			if ((q16 < (r2[i]%q3)) && ((r2[i]%q3) <= q518))
				r2_tmp[i] = q-1;
			else
				r2_tmp[i] = 0;
		}
	}
}

// The lattice bitflip algo
int decode(int n, int q, int q3, int* x, int* y, int* syndrome, int D_thr, int* r1_tmp, int* r2_tmp, int* p_syndrome, int * r1, int * r2, int* result)
{

	// Initialization to 0
	for (int i = 0; i < n; ++i)
	{
		p_syndrome[i] = syndrome[i];
		r1_tmp[i] = r2_tmp[i] = 0;
	}

	for (int i = 0; i < D_thr; ++i)
	{
		setR1(n, q, p_syndrome, result);
		vectorAdd(n, q, r1_tmp, result, r1_tmp);
		setR2(n, q, q3, p_syndrome, result);
		vectorAdd(n, q, r2_tmp, result, r2_tmp);

		for (int j = 0; j < n; ++j)
		{
			if (r1_tmp[j] == 2)
				r1_tmp[j] = q-1;
			if (r1_tmp[j] == q-2)
				r1_tmp[j] = 1;
			if (r2_tmp[j] == 2)
				r2_tmp[j] = q-1;
			if (r2_tmp[j] == q-2)
				r2_tmp[j] = 1;
		}

		// update syndrome 		
		leftMultiplyByH(n,q,x, y, r1_tmp, r2_tmp, result);
		vectorSub(n, q, syndrome, result, p_syndrome);

		if (test(n, q, p_syndrome))
			return 0;
	}
	return -1;
}

// Test the equality of two vectors
int vectorEqual(int n, int* a, int* b)
{
	for (int i = 0; i < n; ++i)
		if (a[i]!=b[i])
			return 0;
	return 1;
}

// Main
int main(int argc, char const *argv[])
{
	// Command help
	if (argc != 6)
	{
		cout << "Usage: ./lattice-bitflip n q D_thr number_of_lattices iterations_per_lattice\n";
		return -1;
	}

	// Parsing
	int n        = atoi(argv[1]);	
	int q        = atoi(argv[2]);	
	int D_thr    = atoi(argv[3]);	
	int num_lat  = atoi(argv[4]);	
	int it_p_lat = atoi(argv[5]);	
	int nb_fail = 0;
	int q3, q9;
	
	// Separating int from frac
	double tmp, fracpart, intpart;
	tmp = ((double)q)/3;
	fracpart = modf(tmp, &intpart);
	if (fracpart <= 0.5)
		q3 = (int) floor(tmp);
	else
		q3 = (int) ceil(tmp);
	tmp = ((double)q)/9;
	fracpart = modf(tmp, &intpart);
	if (fracpart <= 0.5)
		q9 = (int) floor(tmp);
	else
		q9 = (int) ceil(tmp);

	// Allocating memory
	int *x, *y, *r1, *r2, *e, *synd, *r1_tmp, *r2_tmp, *p_syndrome, *tmp_pt;
	if ((x = (int*)malloc(n*sizeof(int)))==NULL)
		return -1;
	if ((y = (int*)malloc(n*sizeof(int)))==NULL)
		return -1;
	if ((r1 = (int*)malloc(n*sizeof(int)))==NULL)
		return -1;
	if ((r2 = (int*)malloc(n*sizeof(int)))==NULL)
		return -1;
	if ((e = (int*)malloc(n*sizeof(int)))==NULL)
		return -1;
	if ((synd = (int*)malloc(n*sizeof(int)))==NULL)
		return -1;
	if ((r1_tmp = (int*)malloc(n*sizeof(int)))==NULL)
		return -1;
	if ((r2_tmp = (int*)malloc(n*sizeof(int)))==NULL)
		return -1;
	if ((p_syndrome = (int*)malloc(n*sizeof(int)))==NULL)
		return -1;
	if ((tmp_pt = (int*)malloc(n*sizeof(int)))==NULL)
		return -1;

	// main loop
	for (int lat=0 ; lat<num_lat ; lat++)
	{	
		// Generate a new lattice
		keyGen(n, q, q3, q9, x, y);
		for (int it=0 ; it<it_p_lat ; it++)
		{
			// Generate a new instance
			noiseGen(n, q, r1, r2, e);
			// Syndrome computation
			computeSyndrome(n,q,x, y, r1, r2, e, synd);
			// Try to recover e from the syndrome and the knowledge of x and y
			int succeed = decode(n, q, q3, x, y, synd, D_thr, r1_tmp, r2_tmp, p_syndrome, r1, r2, tmp_pt);

			if ((succeed==-1) || !(vectorEqual(n,e,p_syndrome)))
				nb_fail++;			

			printf("Lattice n°%d\tIteration n°%d\tNumber of failure(s): %d/%d\n", lat, it, nb_fail, lat*it_p_lat+it+1);
		}
	}
	
	// Freeing
	free(x);
	free(y);
	free(r1);
	free(r2);
	free(e);
	free(synd);
	free(r1_tmp);
	free(r2_tmp);
	free(p_syndrome);
	free(tmp_pt);

	return 0;
}
