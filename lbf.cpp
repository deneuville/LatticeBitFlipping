#include "lbf.hpp"

#define randint(a,b) (rand()%((b)-(a)+1)+(a))
#define randINT(tab) (tab[randint(0,2)])

using namespace std;

void vectorPrint(int n, int* v)
{
	cout << "(";
	for (int i = 0; i < n-1; ++i)
		cout << v[i] << ", ";
	cout << v[n-1] << ")\n";
}

void keyGen(int n, int q, int q3, int q9, int* x, int* y)
{
	srand(time(NULL));

	int tab[3] = {0,1,q-1};

	x[0] = (q3 + randINT(tab))%q;
	y[0] = (q9 + randINT(tab))%q;

	for(int i=1 ; i<n ; i++)
	{
		x[i] = randINT(tab);
		y[i] = randINT(tab);
	}
}

// The decoding matrix adds unnecessary slowness
//int** dec_matrix(int* x, int* y);

// Returns the result of H*(v1, v2) without expanding matrix H = (x | y)
int* leftMultiplyByH(int n, int q, int* x, int* y, int* v1, int* v2)
{

	/* result = 0 */ 
	int * result;
	if ((result=(int*)calloc(n, sizeof(int)))==NULL)
		return NULL;

	/* result += x*v1 */ 
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			if (i<=j)  //+1
				result[i] = (result[i] + x[(n+j-i)%n]*v1[j])%q;
			else  //-1
				result[i] = (result[i] + (q-x[(n+j-i)%n])*v1[j])%q;
		}
	}

	/* result += y*v2 */ 
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			if (i<=j)  //+1
				result[i] = (result[i] + y[(n+j-i)%n]*v2[j])%q;
			else  //-1
				result[i] = (result[i] + (q-y[(n+j-i)%n])*v2[j])%q;
		}
	}

	return result;
}

// Returns vector a+b mod q
int* vectorAdd(int n, int q, int* a, int* b)
{
	int* tmp;
	if ((tmp = (int*)malloc(n*sizeof(int)))==NULL)
		return NULL;
	for (int i = 0; i < n; ++i)
		tmp[i] = (a[i]+b[i])%q;	
	return tmp;
}

// Returns vector a-b mod q
int* vectorSub(int n, int q, int* a, int* b)
{
	int* tmp;
	if ((tmp = (int*)malloc(n*sizeof(int)))==NULL)
		return NULL;
	for (int i = 0; i < n; ++i)
		tmp[i] = (a[i]+(q-b[i]))%q;	
	return tmp;
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
int* computeSyndrome(int n, int q, int* x, int* y, int* r1, int* r2, int* e)
{
	int* mult = leftMultiplyByH(n,q,x,y,r1,r2);
	if (mult==NULL)
	{
		cout << "Error occured during multiplication (in computeSyndrome).\n";
		return NULL;
	}
	return vectorAdd(n, q, mult, e);
}

// Test whether all the coordinates of e are in {-1, 0, 1} or not
int test(int n, int q, int* e)
{
	for (int i = 0; i < n; ++i)
		if ((e[i]!=q-1)&&(e[i]!=0)&&(e[i]!=1))
			return 0;
	return 1;
}

// Test the magnitude of the coordinates
int* testR1(int n, int q, int* r1)
{
	int* r1_tmp;
	if ((r1_tmp = (int*)malloc(n*sizeof(int)))==NULL)
		return NULL;
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
	return r1_tmp;
}

int* testR2(int n, int q, int q3, int* r2)
{
	int* r2_tmp;
	if ((r2_tmp = (int*)malloc(n*sizeof(int)))==NULL)
		return NULL;
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
	return r2_tmp;

}

// The lattice bitflip algo
int* decode(int n, int q, int q3, int* x, int* y, int* syndrome, int D_thr, int* r1_tmp, int* r2_tmp, int* p_syndrome, int * r1, int * r2)
{

	// Initialization to 0
	for (int i = 0; i < n; ++i)
	{
		p_syndrome[i] = syndrome[i];
		r1_tmp[i] = r2_tmp[i] = 0;
	}

	for (int i = 0; i < D_thr; ++i)
	{
		r1_tmp = vectorAdd(n, q, r1_tmp, testR1(n, q, p_syndrome));
		r2_tmp = vectorAdd(n, q, r2_tmp, testR2(n, q, q3, p_syndrome));

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
		
		int* mult = leftMultiplyByH(n,q,x, y, r1_tmp, r2_tmp);

		if (mult==NULL)
		{
			cout << "Error occured during multiplication (in computeSyndrome).\n";
			return NULL;
		}

		p_syndrome = vectorSub(n, q, syndrome, mult);

		if (test(n, q, p_syndrome))
		{
			// Print the number of trials ? 1:yes, 0:no
			//if (1)
			if (0)
				printf("%d/%d trials\n", i, D_thr);
			return p_syndrome;
		}
	}
	return NULL;
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
	int *x, *y, *r1, *r2, *e, *synd, *r1_tmp, *r2_tmp, *p_syndrome;
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

	// main loop
	for (int lat=0 ; lat<num_lat ; lat++)
	{	
		// Generate a new lattice
		keyGen(n, q, q3, q9, x, y);
		for (int it=0 ; it<it_p_lat ; it++)
		{
			// Generate a new instance
			noiseGen(n, q, r1, r2, e);
			synd = computeSyndrome(n,q,x, y, r1, r2, e);
			p_syndrome = decode(n, q, q3, x, y, synd, D_thr, r1_tmp, r2_tmp, p_syndrome, r1, r2);

			if (!(vectorEqual(n,e,p_syndrome)))
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
	return 0;
}
