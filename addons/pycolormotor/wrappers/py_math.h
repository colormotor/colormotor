namespace cm
{

void randomSeed( float seed );

arma::mat velocity( const arma::mat& X_, float dt);
arma::mat acceleration( const arma::mat& X, float dt );
arma::mat jerk( const arma::mat& X, float dt );
arma::vec speed( const arma::mat& X, float dt );

/// Column wise norm
arma::vec col_norm( const arma::mat& X );

arma::vec normalizeSignal( const arma::vec& X );

/// Handy for armadillo matrices
/// 3d transformations
//@{
arma::mat rotX3d( double theta, bool affine=true );
arma::mat rotY3d( double theta, bool affine=true );
arma::mat rotZ3d( double theta, bool affine=true );
arma::mat trans3d( const arma::vec& xyz );
arma::mat trans3d( double x, double y, double z );
arma::mat scaling3d( const arma::vec& xyz, bool affine=true );
arma::mat scaling3d( double s, bool affine=true );
arma::mat scaling3d( double x, double y, double z, bool affine=true );
//@}

/// 2d transformations
//@{
arma::mat rot2d( double theta, bool affine=true );
arma::mat trans2d( const arma::vec& xy, bool affine=true );
arma::mat trans2d( double x, double y, bool affine=true );
arma::mat scaling2d( const arma::vec& xy, bool affine=true );
arma::mat scaling2d( double s, bool affine=true );
arma::mat scaling2d( double x, double y, bool affine=true );
//@}

/// Utils for getting and settings columns of matrix
//@{
void x44( arma::mat& m, const arma::vec& v );
void y44( arma::mat& m, const arma::vec& v );
void z44( arma::mat& m, const arma::vec& v );
void trans44( arma::mat& m, const arma::vec& v );

arma::vec x44( const arma::mat& m );
arma::vec y44( const arma::mat& m );
arma::vec z44( const arma::mat& m );
arma::vec trans44( const arma::mat& m );
    
void x33( arma::mat& m, const arma::vec& v );
void y33( arma::mat& m, const arma::vec& v );
void trans33( arma::mat& m, const arma::vec& v );

arma::vec x33( const arma::mat& m );
arma::vec y33( const arma::mat& m );
arma::vec trans33( const arma::mat& m );
//@}

// Projective/camera transformations
M44 perspective(double fov, double aspect, double znear, double zfar, bool wFriendly=false);
M44 frustum( double left, double right, double bottom, double top, double near, double far );
M44 orthoOffCenter(double l, double r, double b, double t , double zn, double zf );
M44 ortho( double w, double h, double znear, double zfar );
M44 pointAt(const V3 & pos, const V3 & target, const V3 & up );
M44 lookAt(const V3 & pos, const V3 & target, const V3 & up );

// Quaternions
V4 quatMul( const V4&a, const V4& b );
V4 quat( const M44& m );
M44 m44( const V4& q_ );
V4 quatAxisAngle( const V3& axis, double angle );
V4 quatIdentity();
V3 quatAxis( const V4& q, double eps=1e-10 );
V4 quatEuler( double x, double y, double z );
V4 quatEuler( const V3& v );
V4 quatSetAngle( const V4& q, double theta);
double quatAngle( const V4& q );
V4 quatConjugate( const V4& q );
V4 slerp( const V4& a, const V4& b, double t, double minAngle=0.001 );


}