__global__ void dk_xfft8x8_r2c_perm2d( float2* d_c, 
    const __half* __restrict__ d_r, const float* __restrict__ d_RF, 
    unsigned int nx, unsigned int ny, unsigned int ldc, unsigned int ldr, unsigned int n_cells )
{
    const int brev[]={0,2,1,3};
    __shared__ float smem[16*41];
    __shared__ float2 s_RF[4];
    float2 c[5];
    unsigned int tid=threadIdx.x;
    unsigned int bid=blockIdx.x;
    unsigned int x=tid&7;
    unsigned int y=tid>>3;
    unsigned int p=tid&15;
    unsigned int q=tid>>4;
    unsigned int u=x&1;
    unsigned int v=x>>1;
    unsigned int icell=(bid<<4)+y;
    float* spx=&smem[y*40+x];
    float* spy=&smem[y*40+v*10+u];
    d_c+=q*ldc+(bid<<4)+p;
    d_r+=icell*ldr+x;
    if(y==0){ ((float*)s_RF)[x]=d_RF[x]; }
    if(icell<n_cells){
    #pragma unroll
        for( int i=0; i<4; ++i ){
            c[i].x=__half2float(d_r[(i*2+0)*8]);
            c[i].y=__half2float(d_r[(i*2+1)*8]);
        }
    } __syncthreads();
    s_vfft8( c, spx, spy, s_RF, brev );
    s_hfft8( c, &smem[y*40+v*10+u*4], spx, s_RF, brev, x, u );
    s_store5( d_c, &smem[y*41+x], &smem[p*41+q], c, ldc<<3 );
}
__global__ void dk_xfft8x8_r2c_perm2d_ext( float2* d_c, 
    const __half* __restrict__ d_r, const float* __restrict__ d_RF, 
    unsigned int nx, unsigned int ny, unsigned int ldc, unsigned int ldr, unsigned int n_cells )
{
    const int brev[]={0,2,1,3};
    __shared__ float smem[16*41];
    __shared__ float2 s_RF[4];
    float2 c[5];
    unsigned int tid=threadIdx.x;
    unsigned int bid=blockIdx.x;
    unsigned int x=tid&7;
    unsigned int y=tid>>3;
    unsigned int p=tid&15;
    unsigned int q=tid>>4;
    unsigned int u=x&1;
    unsigned int v=x>>1;
    unsigned int icell=(bid<<4)+y;
    float* spx=&smem[y*40+x];
    float* spy=&smem[y*40+v*10+u];
    d_c+=q*ldc+(bid<<4)+p;
    d_r+=icell*ldr+x;
    CLEAR4C(c)
    if(y==0){ ((float*)s_RF)[x]=d_RF[x]; }
    if((icell<n_cells)&(x<nx)){
    #pragma unroll
        for( int i=0; i<4; ++i ){
            if((2*i+0)<ny){ c[i].x=__half2float(*d_r); } d_r+=nx;
            if((2*i+1)<ny){ c[i].y=__half2float(*d_r); } d_r+=nx;
        }
    } __syncthreads();
    s_vfft8( c, spx, spy, s_RF, brev );
    s_hfft8( c, &smem[y*40+v*10+u*4], spx, s_RF, brev, x, u );
    s_store5( d_c, &smem[y*41+x], &smem[p*41+q], c, ldc<<3 );
}
__global__ void dk_xfft8x8_r2c_perm2d_pad( float2* d_c, 
    const __half* __restrict__ d_r, const float* __restrict__ d_RF, 
    unsigned int nx, unsigned int ny, unsigned int ldc, unsigned int ldr, unsigned int n_cells, 
    int pad_x, int pad_y )
{
    const int brev[]={0,2,1,3};
    __shared__ float smem[16*41];
    __shared__ float2 s_RF[4];
    float2 c[5];
    unsigned int tid=threadIdx.x;
    unsigned int bid=blockIdx.x;
    unsigned int x=tid&7;
    unsigned int y=tid>>3;
    unsigned int p=tid&15;
    unsigned int q=tid>>4;
    unsigned int u=x&1;
    unsigned int v=x>>1;
    unsigned int icell=(bid<<4)+y;
    int ox=(int)x-pad_x;
    int oy=-pad_y;  
    float* spx=&smem[y*40+x];
    float* spy=&smem[y*40+v*10+u];
    d_c+=q*ldc+(bid<<4)+p;
    d_r+=icell*ldr+ox;
    CLEAR4C(c)
    if(y==0){ ((float*)s_RF)[x]=d_RF[x]; }
    if((icell<n_cells)&(ox>=0)&(ox<nx)){
    #pragma unroll
        for( int i=0; i<4; ++i ){
            if((oy>=0)&(oy<ny)){ c[i].x=__half2float(*d_r); d_r+=nx; } ++oy;
            if((oy>=0)&(oy<ny)){ c[i].y=__half2float(*d_r); d_r+=nx; } ++oy;
        }
    } __syncthreads();
    s_vfft8( c, spx, spy, s_RF, brev );
    s_hfft8( c, &smem[y*40+v*10+u*4], spx, s_RF, brev, x, u );
    s_store5( d_c, &smem[y*41+x], &smem[p*41+q], c, ldc<<3 );
}
__global__ void dk_xfft8x8_r2c_perm2d_flip( float2* d_c, 
    const __half* __restrict__ d_r, const float* __restrict__ d_RF, 
    unsigned int nx, unsigned int ny, unsigned int ldc, unsigned int ldr, unsigned int n_cells )
{
    const int brev[]={0,2,1,3};
    __shared__ float smem[16*41];
    __shared__ float2 s_RF[4];
    float2 c[5];
    unsigned int tid=threadIdx.x;
    unsigned int bid=blockIdx.x;
    unsigned int x=tid&7;
    unsigned int y=tid>>3;
    unsigned int p=tid&15;
    unsigned int q=tid>>4;
    unsigned int u=x&1;
    unsigned int v=x>>1;
    unsigned int icell=(bid<<4)+y;
    float* spx=&smem[y*40+x];
    float* spy=&smem[y*40+v*10+u];
    d_c+=q*ldc+(bid<<4)+p;
    d_r+=icell*ldr+ny*nx-x-1;
    CLEAR4C(c)
    if(y==0){ ((float*)s_RF)[x]=d_RF[x]; }
    if((icell<n_cells)&(x<nx)){
        c[0].x=__half2float(*d_r);
        c[0].y=__half2float(*(d_r-=nx));
    #pragma unroll
        for( int i=1; i<4; ++i ){
            if((2*i+0)<ny){ c[i].x=__half2float(*(d_r-=nx)); }
            if((2*i+1)<ny){ c[i].y=__half2float(*(d_r-=nx)); }
        }
    } __syncthreads();
    s_vfft8( c, spx, spy, s_RF, brev );
    s_hfft8( c, &smem[y*40+v*10+u*4], spx, s_RF, brev, x, u );
    s_store5( d_c, &smem[y*41+x], &smem[p*41+q], c, ldc<<3 );
}