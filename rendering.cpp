struct Vec3f{
    float x,y,z;
    Vec3f(float x=0,float y=0,float z=0): x(x),y(y),z(z) {}

    Vec3f operator+(const Vec3f& v) const{return Vec3f(x+v.x,y+v.y,z+v.z);}
    Vec3f operator*(float scalar) const{return Vec3f(x*scalar,y*scalar,z*scalar);}
    Vec3f operator*(const Vec3f& v) const{return Vec3f(x*v.x,y*v.y,z*v.z);}

    float dot(const Vec3f& v) const{return x*v.x+y*v.y+z*v.z;}

    Vec3f normalize() const{
        float len=std::sqrt(x*x+y*y+z*z);
        return (len>0)?(*this)*(1.0f/len) : *this;
    }
};
//BDRF Shader
Vec3f myShader(const Vec3f& Wi,const Vec3f& Wo){
    Vec3f albedo(0.8f,0.6f,0.2f);
    float roughness=0.5f;
    float brdf=1.0f/M_PI;
    return albedo*brdf;
}
//Shading point using BRDF
Vec3f shadeP(const Vec3f& ViewDirection,const Vec3f& Point,const Vec3f& SurfaceNormal){
    Vec3f totalReflected(0.0f,0.0f,0.0f);

    std::vector<Vec3f>lightPositions={Vec3f(10,10,10)};
    std::vector<Vec3f>lightColors={Vec3f(1.0f,1.0f,1.0f)};

    for(size_t i=0;i<lightPositions.size();i++){
        Vec3f LightDirection=(lightPositions[i]-Point).normalize();
        Vec3f Wi=LightDirection;
        Vec3f W0=ViewDirection;

        float NdotL=std::max(0.0f,SurfaceNormal.dot(LightDirection));
        Vec3f BRDF =myShader(Wi,Wo);
        Vec3f lightEnergy=lightColors[i];

        totalReflected=totalReflected+(lightEnergy*BRDF*NdotL);
    }
    return totalReflected;
}
Vec3f reflect(const Vec3f& I,const Vec3f& N){
    return I - N*(2.0f*I.dot(N));
}
Vec3f clamp(const Vec3f& v,float minVal=0.0f,float maxVal=1.0f){
    return Vec3f(
        std::max(minVal,std::min(maxVal,v.x)),
        std::max(minVal,std::min(maxVal,v.y)),
        std::max(minVal,std::min(maxVal,v.z))
    );
}
Vec3f fresnelSchlick(float cosTheta,const Vec3f& F0){
    return F0+(Vec3f(1.0f,1.0f,1.0f)-F0)*powf(1,0f-cosTheta,5.0f);
}
float DistributionGCX(const Vec3f& N,const Vec3f& H,float roughness){
    float a=roughness*roughness;
    float a2=a*a;
    float NdotH=std::max(N.dot(H),0.0f);
    float NdotH2=NdotH*NdotH;
    float num=a2;
    float denom=(NdotH2*(a2-1.0f)+1.0f);
    denom=M_PI*denom*denom;

    return num/denom;
}
float GeometrySchlickGGX(float NdotV,float roughness){
    float r=(roughness+1.0f);
    float k=(r*r)/8.0f;
    return NdotV/(NdotV*(1.0f-k)+k);
}
float GeometrySmith(const Vec3f& N,const Vec3f& V,const Vec3f& L,float roughness){
    float NdotV=std::max(N.dot(V),0.0f);
    float NdotL=std::max(N.dot(L),0.0f);
    float ggx1=GeometrySchlickGGX(NdotV,roughness);
    float ggx2=GeometrySchlickGGX(NdotL,roughness);
    return ggx1*ggx2;
}
Vec3f myShader(const Vec3f& Wi, const Vec3f& Wo, const Vec3f& N)
{
    Vec3f albedo(0.8f, 0.6f, 0.2f);
    float metallic = 0.1f;
    float roughness = 0.5f;

    Vec3f H = (Wi + Wo).normalize();
    Vec3f F0 = Vec3f(0.04f, 0.04f, 0.04f); // Dielectric
    F0 = F0 * (1.0f - metallic) + albedo * metallic;

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, Wo, Wi, roughness);
    Vec3f F = fresnelSchlick(std::max(H.dot(Wo), 0.0f), F0);

    Vec3f numerator = F * NDF * G;
    float denominator = 4.0f * std::max(N.dot(Wo), 0.0f) * std::max(N.dot(Wi), 0.0f) + 1e-6f;
    Vec3f specular = numerator * (1.0f / denominator);

    Vec3f kd = Vec3f(1.0f, 1.0f, 1.0f) - F; // Energy conservation
    kd = kd * (1.0f - metallic);

    Vec3f diffuse = albedo * kd / M_PI;

    return diffuse + specular;
}
