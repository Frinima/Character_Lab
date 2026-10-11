#pragma once
#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "PrepareARPGCommandlet.generated.h"

UCLASS()
class UPrepareARPGCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UPrepareARPGCommandlet();
    virtual int32 Main(const FString& Params) override;
};
