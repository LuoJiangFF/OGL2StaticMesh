#ifndef UTILITY_H
#define UTILITY_H

class BimRunGeometry;
class BimRunMaterialGroupInfo;
class BimRunMaterialInfo;
class BimRunSceneModel;
class UMaterialInterface;
class UStaticMesh;

/// <summary>
/// 
/// </summary>
/// <param name="materialInfo"></param>
/// <returns></returns>
UMaterialInterface* CreateMaterialInterface(const BimRunMaterialInfo& materialInfo);

/// <summary>
/// 
/// </summary>
/// <param name="mesh"></param>
/// <param name="buffer"></param>
/// <returns></returns>
UFUNCTION(BlueprintCallable)
int FillStaticMeshWithOGL(UStaticMesh& mesh, uint8* buffer, int32 length);
#endif //STATIC_MESH_CREATOR_H