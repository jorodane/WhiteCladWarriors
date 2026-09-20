# White-Clad Warriors

**플레이 중 스킬과 기믹을 구성·실험하기 위한 UObject 기반 액션 그래프**

Unreal Engine 5.6 · C++ / Blueprint · 커서 기반 실행 · 런타임 규칙 구성

White-Clad Warriors(WCW)는 영웅 조작과 마을 운영을 결합하는 RTS × RPG 개인 개발 프로젝트입니다. 현재는 본게임의 진행과 콘텐츠를 확장하기 위한 기반을 만드는 단계이며, 이 저장소의 주요 탐색 대상은 **행동을 노드로 조립하고, 실행 중인 커서를 통해 이어 가는 액션 그래프 시스템**입니다.

액션 그래프의 중심 목표는 **게임이 실행되는 동안 스킬과 규칙을 만들고, 실험하고, 플레이에 반영하는 경험**입니다. 유즈맵을 세팅하면서 스킬을 시험하거나, 유저 GM과 AI GM이 밸런스를 조정하고 서사에 따라 새로운 기믹·기술을 추가하는 상황을 염두에 두고 설계했습니다. 콘텐츠 제작의 재사용성은 이 경험을 지탱하는 기반입니다.

입력 선택, 이동, 방향 전환, 몽타주, 지연, 반복, 투사체 생성, 피해 처리 등을 `UObject` 노드로 연결합니다. C++은 실행 컨텍스트와 전환 규약을 제공하고, Blueprint는 구체적인 동작과 프리셋을 구성합니다. 노드를 런타임 객체로 유지하여 **인게임 편집 UI, 월드 안의 상호작용, 프로그램에 의한 구성**이 같은 실행 모델을 다룰 수 있도록 하는 것이 주요 설계 선택입니다.

## 빠르게 살펴보기

- **설계 의도부터 읽기:** [플레이 중 규칙을 구성하는 UX](#설계-목표-플레이-중-규칙을-구성하는-ux) → [왜 UObject 노드인가](#왜-uobject-노드인가)
- **실행 구조부터 읽기:** [액션 그래프의 구성](#액션-그래프의-구성) → [커서를 보유하는 노드](#커서를-보유하는-노드) → [값의 범위와 Claimer](#값의-범위와-claimer)
- **코드부터 읽기:** [주요 코드 탐색 경로](#주요-코드-탐색-경로)의 순서대로 정의·실행기·노드·값 저장소 확인
- **Unreal Editor에서 보기:** [Blueprint 노드와 프리셋](#blueprint-노드와-프리셋)의 `BP_Action_Move`, `BP_Action_BasicAttack`, `BP_Action_FireBall`
- **사용 경험 살펴보기:** [유즈맵과 GM의 활용 시나리오](#유즈맵과-gm의-활용-시나리오)와 [확장 가능성](#확장-가능성)
- **프로젝트 열기:** [실행 환경](#실행-환경)

## 프로젝트와 현재 범위

게임의 목표는 영웅의 성장과 마을 운영을 병행하며 마왕 처치를 겨루는 플레이입니다. 이 README는 그 기획 전체를 구현 완료 목록으로 소개하지 않고, 현재 저장소에서 확인할 수 있는 액션 기반에 집중합니다.

**현재 구현의 중심은 노드·커서·실행기와 Blueprint 프리셋입니다.** 아래의 인게임 편집 UX, 물리적으로 조작하는 노드, 유저 GM·AI GM 연동은 이 기반이 지향하는 사용 경험이며, 완성된 기능으로 소개하지 않습니다.

| 구분 | 현재 저장소에서 살펴볼 내용 |
| --- | --- |
| 액션 실행 기반 | 노드 연결, 커서 관리, 입력 전달, 완료·취소·진입 거부 처리, 서브 커서 생성 |
| 실행 데이터 | 실행기 내부의 계층형 값 저장소, 위치·방향·대상을 계산하는 Claimer |
| 런타임 구성 접점 | Blueprint에서 다룰 수 있는 UObject 노드, 쓰기 가능한 노드 연결·액션 진입점, 이름 있는 연결 추가 API |
| 콘텐츠 조립 | 이동·공격·대시·투사체 계열 Blueprint 프리셋과 지연·반복·동시 실행 노드 |
| 게임 연결 | 유닛 행동 컴포넌트, 공격 실행, 몽타주 메시지, 입력 인디케이터, 실행기와 표시 액터의 풀링 |
| 주변 기반 | 유닛 선택·명령 예약, UI, 값 컨테이너, 인벤토리 관련 코드 |
| 본게임 콘텐츠 | 마을 운영, 성장·경제, 한 판의 진행과 승리 조건 등은 개발 목표이며 완성된 플레이 루프로 소개하지 않음 |

여기서 **액션 그래프**는 Blueprint에서 생성·연결하는 `UObject` 노드들의 실행 구조를 뜻합니다. 노드 객체를 명령, 커서를 실행 위치, 실행기를 컨텍스트로 보면 객체 기반 인터프리터의 관점으로도 읽을 수 있습니다. 별도의 바이트코드 형식이나 범용 VM을 제공하는 프로젝트라는 의미는 아닙니다.

## 설계 목표: 플레이 중 규칙을 구성하는 UX

스킬의 발사 횟수와 피해량을 바꾸는 것부터, 새로운 조건·반복·후속 효과를 연결하는 것까지 게임 안의 조작 대상으로 삼고자 합니다. 사용자가 규칙을 구성하고 결과를 확인하는 과정 자체가 유즈맵 제작과 GM 플레이의 일부가 됩니다.

| 상황 | 사용자가 하고 싶은 일 | 액션 그래프가 맡을 역할 |
| --- | --- | --- |
| 유즈맵 세팅 | 스킬을 조립하고 테스트 유닛으로 사용해 보며 횟수·간격·방향을 조정 | 구성한 노드와 연결을 실행에 사용하고, 반복 실험의 단위를 제공 |
| 유저 GM의 밸런스 조정 | 진행 중인 전투에 맞추어 스킬의 위력·주기·패턴을 변경 | 수치 설정과 행동 흐름의 변경을 함께 표현 |
| 서사에 따른 규칙 추가 | 사건으로 얻은 기술이나 보스의 새로운 기믹을 그 장면에서 구체화 | 조건·동작·메시지·후속 효과를 조합하여 실행 가능한 규칙으로 구성 |
| AI GM의 개입 | 상황과 서사에 맞는 변화안을 만들고 게임 동작에 반영 | AI 측 구성 계층이 사용할 노드 종류·파라미터·연결의 실행 모델 제공 |

여기서 중요한 설계 원칙은 **편집 대상과 실행 대상이 같은 노드 모델을 사용한다는 것**입니다. UI에서 선택한 노드, 프로그램이 생성한 노드, 월드 액터가 참조하는 노드가 공통된 `UActionNode` 규약을 따르면, 조작 방식이 달라져도 연결과 실행의 의미를 유지할 수 있습니다.

이를 위해 행동의 종류를 정의하는 **노드 클래스**, 특정 스킬을 구성하는 **노드 인스턴스와 연결**, 한 번의 사용을 진행하는 **실행기와 커서**를 나누어 다룹니다. 새 동작의 종류는 C++ 또는 Blueprint 파생 클래스로 준비하고, 런타임에서는 준비된 종류의 인스턴스를 만들고 연결하여 구체적인 스킬을 구성하는 방향입니다.

## 왜 UObject 노드인가

`UObject`는 이 설계에서 **런타임 생성, 참조에 의한 연결, UI와 월드에서의 표현**을 이어 주는 공통 객체입니다. 특정 편집 화면의 수명이나 표시 방식에 노드의 의미가 묶이지 않도록 선택했습니다.

### 런타임에 생성하고 연결하는 단위

[`UActionNode`](Source/WhiteCladWarriors/Public/Actions/Executables/ActionNode.h)는 `UCLASS(Blueprintable, BlueprintType)`으로 선언되어 있습니다. Blueprint에서 파생 동작을 만들고 객체 참조를 주고받을 수 있으며, 인게임 구성 계층에서 준비된 노드 클래스를 `Construct Object from Class`로 인스턴스화하는 방식을 사용할 수 있습니다. 이 엔진 API는 생성할 클래스와 `Outer`를 받아 새 객체를 반환합니다. [Epic 문서: Construct Object from Class](https://dev.epicgames.com/documentation/en-us/unreal-engine/BlueprintAPI/Game/ConstructObjectfromClass)

| 현재 공개된 접점 | 런타임 구성에서의 의미 |
| --- | --- |
| `NextNode`, `CanceledNode`, `BlockedNode`의 `BlueprintReadWrite` | 생성한 노드를 정상·취소·진입 거부 경로에 연결 |
| `LinkedNodes`와 `AddNodeLink` | 이름 있는 사건에 후속 노드를 연결하거나 기존 이름의 연결을 변경 |
| `FLinkedNodeInfo::bIsSubNode` | 같은 커서의 전환과 추가 실행 흐름 생성을 구분 |
| `AActionBase::RootNode`, `RootAsSubNode`의 `BlueprintReadWrite` | 구성한 그래프를 액션의 진입점으로 지정 |
| Blueprint 이벤트와 파생 클래스 | 노드의 실행·완료·취소·메시지 처리 규약을 확장 |

실행 중 구성할 때의 기본 단위는 **객체 생성 → 파라미터·Claimer 설정 → 노드 참조 연결 → 액션 진입점 지정**입니다. 이 접점들이 인게임 편집기의 기반이 되며, 노드 선택 화면·연결 조작·변경 이력 같은 사용자 경험은 그 위에 구현할 영역입니다.

### Widget과 ListView가 같은 노드를 다루는 방식

노드 객체를 Widget의 참조 변수에 등록하면, Widget은 해당 노드의 값을 표시하고 편집 요청을 전달하는 역할을 맡을 수 있습니다. 목록형 UI에서도 노드 객체 자체를 항목으로 사용할 수 있습니다. Unreal의 `UListView`는 `UObject` 항목을 받으며, 화면에 표시하는 Entry Widget은 `IUserObjectListEntry`를 통해 항목을 다룹니다. [Epic 문서: UListView](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/UMG/UListView)

| 표현 방식 | 노드 객체와의 관계 | 구상하는 조작 |
| --- | --- | --- |
| 상세 편집 Widget | 선택된 노드 인스턴스를 참조 | 노드 종류에 맞는 수치·대상·Claimer 설정 편집 |
| ListView | 노드 인스턴스를 목록의 Item으로 전달 | 단계 선택, 노드 목록 탐색, 선택 항목의 상세 표시 |
| 연결을 보여 주는 그래프 UI | 화면상의 노드와 `UActionNode` 참조를 대응 | 연결 추가·교체, 분기와 후속 효과 구성 |

예를 들어 목록에서 고른 반복 노드와 상세 패널이 **같은 인스턴스**를 참조하게 만들 수 있습니다. 패널의 편집 결과를 그 노드에 반영하고 필요한 표시 갱신을 연결하면, 실행에 사용할 노드와 UI의 편집 대상을 일치시킬 수 있습니다. ListView의 표시 순서가 실행 순서를 자동으로 바꾸지는 않으므로, 순서 변경을 제공할 때는 실제 노드 연결까지 갱신하도록 구성해야 합니다.

노드의 보유 주체와 수명도 편집 화면과 함께 설계합니다. UI를 닫아도 사용할 그래프는 액션이나 별도 구성 관리 객체가 참조를 유지하고, Widget은 필요한 동안 이를 표시하도록 구성할 수 있습니다. UObject를 사용하면 이런 참조 관계를 활용할 수 있지만, 필드 편집 UI나 변경 알림이 자동으로 완성되는 것은 아닙니다.

### 월드에서 조작하고 물리적으로 반응하는 노드

같은 노드 객체를 Actor의 프로퍼티로 보유하게 하면, **노드의 의미는 UObject에 두고 공간상의 표현과 물리 상호작용은 Actor·Component에 맡기는 방식**으로 확장할 수 있습니다. Actor의 위치·회전·충돌·상호작용 결과를 노드의 값이나 연결, 실행 메시지로 전달하는 구조입니다.

가령 발사 방향을 나타내는 월드 오브젝트를 회전시켜 방향 설정을 바꾸거나, 노드를 나타내는 장치를 연결해 후속 효과를 구성하거나, 충돌 사건을 특정 노드의 실행 조건으로 전달하는 UX를 구상할 수 있습니다. 이때 UObject 자체에 물리가 생기는 것은 아니며, 이를 보유한 Actor와 Component가 물리적 역할을 수행합니다.

현재 구현에는 생성 액터와 실행 문맥을 연결하는 `IActionSpawnable`, `ClaimActor`, 충돌 결과를 읽는 Claimer가 있습니다. 이들은 액션과 월드 사이의 기존 접점입니다. **월드에서 노드를 직접 배치·연결하는 편집 UX는 별도의 확장 구상**이며, 아래의 [투사체 수명 관리](#투사체와-실행기의-수명)와 구분해서 읽을 수 있습니다.

이 문서에서 작업 환경에 구애받지 않는다는 것은 **Unreal 안에서 Blueprint 구성, 인게임 Widget·ListView, 월드 Actor, 프로그램에 의한 구성이 공통 노드 모델을 사용할 수 있다는 의미**입니다. 표현과 입력 방식은 각 환경에 맞게 구현하면서, 노드의 연결·실행 규약을 공유하는 방향입니다.

## 주요 코드 탐색 경로

아래 순서는 액션 하나가 시작되어 실행되고 종료되는 과정을 따라갑니다. 공개 API는 `Public`, 실제 전환 로직은 `Private`에 있습니다.

| 순서 | 파일 | 확인할 지점 |
| --- | --- | --- |
| 1 | [ActionBase.h](Source/WhiteCladWarriors/Public/Actions/ActionBase.h), [ActionBase.cpp](Source/WhiteCladWarriors/Private/Actions/ActionBase.cpp) | 액션 정의, `RootNode` / `RootAsSubNode`, `ExecuteAction*` 진입점 |
| 2 | [ActionStructures.h](Source/WhiteCladWarriors/Public/Generals/Structs/ActionStructures.h) | `FActionCursorFinder`, `FActionExecuteSettingContainer`, `FMainActionInfo` |
| 3 | [ActionExecutor.h](Source/WhiteCladWarriors/Public/Actions/Executables/ActionExecutor.h), [ActionExecutor.cpp](Source/WhiteCladWarriors/Private/Actions/Executables/ActionExecutor.cpp) | `CursorMap`, `EnterNode`, `CreateSubNode`, `EndNode`, `CheckCursorMap` |
| 4 | [ActionNode.h](Source/WhiteCladWarriors/Public/Actions/Executables/ActionNode.h), [ActionNode.cpp](Source/WhiteCladWarriors/Private/Actions/Executables/ActionNode.cpp) | 노드의 실행·전환·메시지 규약 |
| 5 | [ActionValueContainer.h](Source/WhiteCladWarriors/Public/Generals/Structs/ActionValueContainer.h), [ActionValueContainer.cpp](Source/WhiteCladWarriors/Private/Generals/ActionValueContainer.cpp) | 실행 데이터의 저장과 부모 범위 조회 |
| 6 | [ActionValueClaimer.h](Source/WhiteCladWarriors/Public/Actions/Values/ActionValueClaimer.h), [ActionValueClaimer.cpp](Source/WhiteCladWarriors/Private/Actions/Values/ActionValueClaimer.cpp) | 위치·방향·대상·거리의 계산과 재사용 |
| 7 | [Blueprint 프리셋](Content/1_Core/Blueprints/Actions/Presets) | 위의 C++ 규약으로 실제 콘텐츠를 구성하는 부분 |

입력에서 진입하는 과정을 보려면 [Operator.cpp](Source/WhiteCladWarriors/Private/Objects/Players/Operator.cpp)의 `CommandAction`을, 유닛의 공격이 다른 액션을 호출하는 과정을 보려면 [UnitAttackComponent.cpp](Source/WhiteCladWarriors/Private/Objects/Selectables/Components/UnitAttackComponent.cpp)의 `ExecuteAttack_Implementation`을 이어서 읽으면 됩니다.

## 액션 그래프의 구성

| 구성 요소 | 역할 |
| --- | --- |
| `AActionBase` | 액션 이름·아이콘·단축키 등의 정보와 그래프 진입점을 보유합니다. |
| `UActionNode` | 한 단계의 실행과 다음 연결을 정의합니다. 실제 동작은 C++ 또는 Blueprint 파생 노드에서 구현합니다. |
| `UActionExecutor` | 한 번의 액션 실행에 필요한 커서, 값 저장소, 생성 액터 목록을 관리합니다. |
| `FActionCursorFinder` | 어느 실행기의 어느 컴포넌트에서 어떤 실행 흐름을 찾을지 식별합니다. 마우스 커서와는 다른 개념입니다. |
| `FActiveNodeMap` / `FActiveNodeInfo` | 컴포넌트별 활성 노드, 메시지 수신 상태, 값 범위 ID와 종료 이벤트를 저장합니다. |
| `UUnitActionComponent` | 액션을 유닛에 연결하고 메인 행동, 입력 준비 상태, 액터 생성 등의 접점을 제공합니다. |
| `AOperator` | 선택된 대상과 플레이어 입력을 모으고 액션 명령·입력 요청을 처리합니다. |
| `UActionSetting` | 액션 이름 조회와 실행기 풀을 관리합니다. |

```mermaid
flowchart TD
    A["AActionBase: 그래프 정의"] --> E["UActionExecutor: 실행 컨텍스트"]
    E --> M["컴포넌트별 CursorMap"]
    M --> C["커서 ID와 현재 노드"]
    C --> N["UActionNode: 실행과 대기"]
    N -->|"다음 노드로 전환"| C
    N -->|"서브 커서 생성"| M
    E --> V["ValueContainer: 실행 데이터"]
    N -->|"커서로 값 조회·저장"| V
    E --> S["CreatedActors: 생성 액터 추적"]
```

커서는 노드 포인터 자체를 보관하는 대신 `CurrentExecutorID`, `CurrentComponent`, `CurrentID`로 실행 중인 노드를 조회합니다. `CurrentAction`, `CurrentOperator`는 액션의 출처를 전달하고, `ClaimActor`는 생성 액터 등 현재 문맥의 행위자를 전달하는 데 사용됩니다.

이 구분 덕분에 액션의 연결 구조와 **이번 실행에서 어느 단계에 있는지**를 따로 다룰 수 있습니다. 동시에 노드 내부에 상태가 필요한 경우도 있습니다. 예를 들어 지연 노드는 하나의 남은 시간 변수를 공유하지 않고, 커서를 키로 하는 타이머 목록을 사용합니다.

런타임 편집 UX에서도 이 구분이 중요합니다. 편집하는 그래프와 진행 중인 실행을 각각 식별할 수 있어야 변경을 다음 시전부터 적용할지, 특정 활성 흐름에도 적용할지 정할 수 있습니다. 현재 구조는 노드 객체를 참조하여 실행하며, 그래프 버전별 스냅샷이나 변경 적용 시점을 일괄 관리하는 계층은 별도로 구성해야 합니다.

## 실행과 전환

일반적인 플레이어 명령 경로는 다음과 같습니다.

1. `AOperator::CommandAction`이 선택된 컴포넌트와 액션을 받습니다.
2. 루트가 Selector라면 입력 필요 여부를 확인합니다. 일반 입력은 `FInputClaim`으로 선택을 요청하고, 즉시 실행 경로는 현재 입력을 검사합니다.
3. `AActionBase::ExecuteAction` 또는 `ExecuteActionWithInput`이 실행 가능한 컴포넌트를 추리고 실행기를 요청합니다.
4. `UActionExecutor`가 컴포넌트별 실행 정보를 등록하고, `ExecuteCursor*`를 통해 시작 노드의 `ClaimExecute*`를 호출합니다.
5. 노드가 작업을 수행하거나 커서를 보유하며 기다립니다. 다음 단계로 갈 때 전환 함수를 호출합니다.
6. 후속 전환의 `EnterNode`는 진입 가능 여부와 메인 행동 설정을 확인하고 대상 노드를 실행합니다.
7. 끝난 커서는 제거됩니다. 활성 커서와 추적 중인 생성 액터가 모두 없어지면 실행기가 풀로 반환됩니다.

`UActionNode`의 연결에는 각각 다른 의미가 있습니다.

| 연결·API | 의미 |
| --- | --- |
| `NextNode` / `MoveExecutorToNext` | 완료 콜백을 호출하고 정상 후속 노드로 진행 |
| `CanceledNode` / `MoveExecutorToCancel` | 취소 콜백을 호출하고 취소 후속 노드로 진행 |
| `BlockedNode` / `GetCanEnter` | 진입 조건을 만족하지 못했을 때 사용할 대체 경로 |
| `LinkedNodes` / `MoveExecutorToLinkedNode` | `FName`으로 이름 붙인 연결. 현재 커서를 옮기거나 서브 커서를 생성 |
| `CreateSubNode` | 기존 문맥에서 추가 실행 흐름을 생성 |
| `MoveExecutorToInterrupt` | 다른 행동에 의한 중단을 통지하고 취소 경로로 진행 |

단순 메시지의 기본 수신 구현은 메시지 이름으로 `LinkedNodes`를 조회합니다. 따라서 노드는 외부에서 받은 사건을 이름 있는 연결로 전달할 수 있습니다. 해당 이름의 연결이 없으면 기본 구현은 커서를 그대로 유지합니다. 한편 `NextNode` 등의 전환 대상이 유효하지 않거나 진입 가능한 후속 노드를 찾지 못한 경우의 종료 처리는 `EnterNode`와 `EndNode`에서 확인할 수 있습니다.

### 커서를 보유하는 노드

`ClaimExecute`가 호출됐다는 사실만으로 다음 노드로 이동하지 않습니다. **현재 동작이 언제 끝나는지는 해당 노드가 결정합니다.** 기본 `UActionNode::ClaimExecute_Implementation` 역시 자동 진행을 수행하지 않습니다.

| 노드가 기다리는 것 | 구현 접점 | 다음 흐름을 결정하는 시점 |
| --- | --- | --- |
| 위치·방향·대상 입력 | `UActionSelectorNode::ReceiveInput`, `FSelectorInput::OnInputAccepted` | 입력 수신 결과에 따라 연결된 노드로 전환 |
| 일정 시간 또는 반복 주기 | `UActionDelayNode::StartTimer`, `OnActivated`, `OnFinishTimer` | 타이머 이벤트를 Blueprint 노드의 전환·반복 처리에 연결 |
| 몽타주 관련 사건 | `OnActionMessage_Montage`, `OnActionMessage_Detail` | 몽타주 노드가 시작·종료·Notify에 따른 흐름을 처리 |
| 사용자 정의 조건 | `ClaimExecute*`, `OnActionMessage_*`, 전환 API | 파생 노드가 자신의 완료·취소 조건을 만족했을 때 |

[ActionDelayNode](Source/WhiteCladWarriors/Private/Actions/Executables/ActionDelayNode.cpp)는 `TMap<FActionCursorFinder, FDelayInfo>`에 실행별 타이머와 반복 횟수를 보관합니다. `BP_DelayNode`, `BP_RepeatNode`는 이 기반을 구체적인 지연·반복 동작으로 연결합니다.

이 구조에서는 새 대기 방식을 추가할 때 실행기에 모든 종류의 대기 조건을 나열할 필요가 없습니다. 새 노드가 자신의 이벤트를 구독하고, 필요한 만큼 커서를 유지한 뒤 전환 규약을 따르면 됩니다. 대신 해당 노드가 **취소 시 타이머·이벤트 구독을 정리하고, 완료 후 중복으로 진행하지 않도록 책임져야 합니다.**

### 메시지, 메인 행동과 서브 커서

유닛 컴포넌트의 메시지는 실행기에 전달되고, `FActiveNodeMap`은 수신 상태가 `Listening`인 활성 노드에 이를 전달합니다. 단순 이름 메시지, 이름과 문맥을 함께 전달하는 상세 메시지, 몽타주 메시지가 나뉘어 있습니다. 몽타주 Notify가 상세 메시지로 변환되는 경로는 [UnitMainComponent.cpp](Source/WhiteCladWarriors/Private/Objects/Selectables/Components/UnitMainComponent.cpp)의 `MontageNotifyBegin` / `MontageNotifyEnd`에 있습니다.

유닛의 **메인 행동**과 추가로 실행하는 **서브 커서**도 구분합니다. `FMainActionInfo`는 현재 메인 행동과 취소 가능 여부를 보관하며, `UActionBehaviorNode::Settings`와 `UUnitMainComponent::SetMainAction`이 새 행동 진입과 기존 행동 중단을 연결합니다.

서브 커서는 다른 실행 흐름을 진행하면서 별도의 노드를 활성화하는 데 사용됩니다. `FLinkedNodeInfo::bIsSubNode`와 `CreateSubNode`가 이 구분을 제공합니다. 여기서 동시 실행은 **여러 커서가 진행되는 논리적 동시성**이며, 작업을 여러 CPU 스레드로 분배한다는 뜻은 아닙니다. 분기 전체를 기다리는 합류 지점이나 취소 범위는 조립하는 노드에서 명시적으로 설계해야 합니다.

또한 유닛의 **명령 예약 큐**와 액션 내부의 그래프는 서로 다른 계층입니다. 명령 예약은 이동 후 공격 같은 액션 간 순서를 관리하고, 그래프는 각 액션 안의 입력·모션·타이밍·효과를 구성합니다.

## 값의 범위와 Claimer

### 실행 중인 값을 어디에 저장하는가

`UActionExecutor::ValueContainer`는 `FInstancedPropertyBag`으로 값을 보관합니다. 키는 범위 ID와 `FName` 태그로 구성하며, 값 조회 시 현재 범위에서 타입에 맞는 값을 찾고 없으면 부모 범위로 올라갑니다.

| 범위 | 생성·조회 방식 |
| --- | --- |
| 루트 | `FActionValueContainer::RootID` |
| 컴포넌트 | `Registration(UUnitActionComponent*)`로 등록 |
| 실행 커서 | 활성 노드 정보가 자신의 `ValueID`를 보유 |
| 서브 커서 | 부모 커서의 `ValueID`를 부모로 등록한 새 범위 사용 |

**값의 상속은 부모 데이터의 복사가 아니라 조회 경로의 연결**입니다. 서브 커서에 같은 태그와 타입의 값을 쓰면 그 범위에서는 새 값을 먼저 읽고, 별도로 쓰지 않은 값은 부모 범위에서 찾습니다.

예를 들어 조준 위치는 부모 문맥에서 받아 오되, 각 발사 분기의 방향은 개별 범위에 저장하도록 구성할 수 있습니다. 이때 부모 값을 수정하면 별도로 덮어쓰지 않은 하위 범위의 이후 조회에도 영향을 줍니다. 발사 순간의 값을 고정해야 한다면 필요한 값을 해당 범위에 저장해야 합니다.

### 값 자체와 값을 구하는 규칙을 분리

`Claimer`는 커서를 받아 현재 문맥에서 값을 구하는 객체입니다. 노드가 모든 대상 선택·좌표 계산을 직접 수행하는 대신, 필요한 규칙을 조합해 전달받도록 합니다.

| 필요한 값 | 구현 예 | 활용 |
| --- | --- | --- |
| 자신의 위치·소켓 위치 | `UPositionClaimer_SelfPosition`, `UPositionClaimer_SocketPosition` | 시전자 중심 효과, 손·무기 소켓에서 발사 |
| 저장된 위치·방향 | `UPositionClaimer_SavedPosition`, `UDirectionClaimer_SavedDirection` | 입력으로 확정한 지점, 분기별 발사 방향 재사용 |
| 다른 액터의 현재 위치 | `UPositionClaimer_ActorPosition` + `UActorClaimer` | 대상 위치를 조회하는 순간에 계산 |
| 두 위치로부터의 방향 | `UDirectionClaimer_ToPosition` | 출발점과 도착점을 각각 다른 규칙으로 구성 |
| 충돌 위치·법선·액터 | `UPositionClaimer_HitPosition`, `UDirectionClaimer_HitNormal`, `UActorClaimer_HitActor` | 충돌 결과를 후속 효과의 입력으로 사용 |
| 현재 문맥의 액터 | `UActorClaimer_TriggerActor` | `ClaimActor`를 우선 사용하고, 없으면 실행 컴포넌트의 소유자 사용 |
| 거리·상수·분포 | `UFloatClaimer_PositionDistance`, `UFloatGetter_*`, `UVectorGetter_*` | 거리 기반 수치, 고정값 또는 분포에서 값 생성 |
| 유닛에 연결된 액션 | `UActionClaimer_UnitTagged` | 유닛의 태그 매핑을 거쳐 사용할 액션 조회 |

위치의 추가 오프셋에는 Self / World 공간을 지정할 수 있고, 방향에는 각도 오프셋을 적용할 수 있습니다. 따라서 같은 발사 노드도 **어디서, 어느 방향으로, 누구를 기준으로 실행할지**를 바꾸어 재사용할 수 있습니다.

인게임 편집에서는 이 구분을 사용자에게 의미 있는 선택으로 표현할 수 있습니다. 예를 들어 발사 위치를 “시전자 위치”, “무기 소켓”, “맞은 지점” 중에서 고르게 하여 해당 Claimer를 구성하는 방식입니다. 좌표 계산의 세부 구현을 매번 편집하게 하지 않으면서도, 스킬이 공간을 해석하는 규칙을 바꿀 수 있도록 하는 접점입니다.

## 투사체와 실행기의 수명

투사체를 생성한 뒤 시전 동작이 끝나더라도, 투사체는 월드에 남아 충돌하거나 후속 액션을 일으킬 수 있습니다. 이 때문에 실행기의 종료 조건은 커서 목록뿐 아니라 생성 액터 목록도 함께 봅니다.

1. `UActionSpawnNode::InstanceRegistration`이 생성 액터를 실행기에 등록합니다.
2. `UActionExecutor::AddCreatedActor`가 `CreatedActors`에 추가합니다.
3. 액터가 `IActionSpawnable`을 구현하면 `SpawnInitialize`로 생성 노드, 커서, 종료 통지 델리게이트를 전달합니다. 전달되는 커서의 `ClaimActor`에는 생성 액터가 들어갑니다.
4. 액터 측에서 종료를 통지하면 `RemoveCreatedActor`가 목록에서 제거합니다.
5. `CheckCursorMap`은 `CursorMap`과 `CreatedActors`가 모두 비었을 때 실행기를 반환합니다.

관련 구현은 [ActionSpawnNode.cpp](Source/WhiteCladWarriors/Private/Actions/Executables/ActionSpawnNode.cpp), [ActionSpawnable.h](Source/WhiteCladWarriors/Public/Interfaces/ActionSpawnable.h), [BP_ProjectileBase](Content/1_Core/Blueprints/Actors/EffectAreas/BP_ProjectileBase.uasset)에 있습니다. 이 경로는 액터의 파괴 통지가 제대로 연결되어야 성립하므로, 새 생성 액터를 추가할 때는 생성뿐 아니라 종료 통지도 함께 구현해야 합니다.

실행기 풀은 [ActionSetting.cpp](Source/WhiteCladWarriors/Private/Settings/ActionSetting.cpp)가 관리합니다. 활성화 시 실행 ID를 발급하고, 반환 시 커서·생성 액터 참조·실행 데이터를 초기화하여 다시 사용합니다.

## 입력 인디케이터와 실행의 연결

스킬 선택 중에 보여 주는 방향·범위도 노드와 연결되어 있습니다. Selector는 미리 보기에 참여할 `IndicatorNodes`를 보유하고, 각 Behavior 노드는 필요한 표시 유형과 수량을 요청합니다.

| 단계 | 구현 |
| --- | --- |
| 입력 요구 전달 | `FInputClaim`에 액션 커서, 대상 컴포넌트, Selector, 설명과 커서 종류 등을 전달 |
| 표시 요청 수집 | `UActionSelectorNode::GetIndicatorClaim`, `UActionBehaviorNode::GetIndicatorClaim` |
| 표시 액터 확보 | `UActionIndicatorBase::ReceiveInputClaim`에서 유형별 풀 사용 |
| 표시 갱신 | 현재 `FInputPackage`를 전달해 `UpdateIndicatorArray` / `UpdateIndicatorSingle` 호출 |
| 입력 종료 후 정리 | 표시 액터를 풀로 반환하고 현재 요청 초기화 |

[ActionIndicatorBase.cpp](Source/WhiteCladWarriors/Private/Actions/Indicators/ActionIndicatorBase.cpp)에는 입력 선택 중 표시와 액션 아이콘 미리 보기 경로가 있습니다. [표시 액터 Blueprint](Content/1_Core/Blueprints/Actors/ActionIndicatorShower)에는 Arrow, Circle, Quad, Range 구현이 포함되어 있습니다.

이 구조는 실제 행동을 구성한 노드가 자신의 미리 보기에도 참여하게 합니다. 인게임 구성 UX에서는 범위·방향 설정을 바꾼 뒤 표시를 확인하고 실제 실행으로 시험하는 반복 과정에 활용할 수 있습니다. 현재의 인디케이터는 액션 입력과 미리 보기의 기반이며, 그래프 편집기의 변경 반영·실행 추적 화면은 추가로 연결할 부분입니다. 실행과 표시가 일치하도록 새 노드의 표시 구현도 동일한 좌표·방향·범위 규칙을 사용해야 합니다.

## Blueprint 노드와 프리셋

구체적인 콘텐츠 로직은 `.uasset`에 포함되어 있습니다. GitHub에서는 경로를 확인하고, 내부 그래프와 세부 파라미터는 Unreal Editor에서 열어 보는 것이 좋습니다.

### 노드 지도

기준 경로: [`Content/1_Core/Blueprints/Actions/Nodes`](Content/1_Core/Blueprints/Actions/Nodes)

| 분류 | 대표 노드 | 디렉터리 |
| --- | --- | --- |
| 입력 선택 | `BP_SelectorNode_Single` | [SelectorNode](Content/1_Core/Blueprints/Actions/Nodes/SelectorNode) |
| 유닛 행동 | `BP_BehaviorNode_MoveToActor`, `MoveToLocation`, `LookAt`, `LaunchCharacter`, `AttackTarget`, `Damage`, `MontagePlay`, `StartAction` | [BehaviorNode](Content/1_Core/Blueprints/Actions/Nodes/BehaviorNode) |
| 시간·반복 | `BP_DelayNode`, `BP_RepeatNode`, `BP_RepeatNode_ShiftAngle` | [DelayNode](Content/1_Core/Blueprints/Actions/Nodes/DelayNode) |
| 동시 실행 | `BP_SimpleNode_ParallelExecution` | [SimpleNode/Branchs](Content/1_Core/Blueprints/Actions/Nodes/SimpleNode/Branchs) |
| 생성 | `BP_SpawnNode_Projectile`, `BP_SpawnNode_Effect` | [SpawnNode](Content/1_Core/Blueprints/Actions/Nodes/SpawnNode) |
| 실행 값 설정 | `BP_ValueSetterNode_SetPosition`, `BP_ValueSetterNode_SetDirection` | [ValueSetterNode](Content/1_Core/Blueprints/Actions/Nodes/ValueSetterNode) |

표의 `BehaviorNode` 행은 공통 접두사 `BP_BehaviorNode_`를 일부 생략했습니다. 조립 보조 매크로는 [Macros](Content/1_Core/Blueprints/Macros)의 `BPM_ActionSample`, `BPM_ActionNodeFunctionMacros`, `BPM_ValueMacros`에서도 확인할 수 있습니다.

### 살펴볼 프리셋

다음 표는 저장소에 포함된 프리셋과 참조하는 노드 계열을 안내합니다. 에셋 참조 목록은 전체 실행 순서와 같지 않으므로, 정확한 분기 연결과 타이밍은 해당 Blueprint에서 확인해야 합니다.

| 프리셋 | 탐색 포인트 |
| --- | --- |
| [BP_Action_Move](Content/1_Core/Blueprints/Actions/Presets/BP_Action_Move.uasset) | Selector와 액터·위치 이동 노드를 연결하는 기본 구성 |
| [BP_Action_Dash](Content/1_Core/Blueprints/Actions/Presets/BP_Action_Dash.uasset) | 입력, 바라보기, 캐릭터 발사, 몽타주 노드의 조합 |
| [BP_Action_BasicAttack](Content/1_Core/Blueprints/Actions/Presets/BP_Action_BasicAttack.uasset) | 대상 선택·방향·몽타주·피해, `RootNode`와 `RootAsSubNode` 진입점 |
| [BP_Action_FireBall](Content/1_Core/Blueprints/Actions/Presets/BP_Action_FireBall.uasset) | Selector, 몽타주, 지연, 반복, 각도 변경, 동시 실행, 투사체·효과 생성 노드를 함께 살펴볼 대표 예제 |
| [BP_Action_BlowArea](Content/1_Core/Blueprints/Actions/Presets/BP_Action_BlowArea.uasset) | 각도 변경 반복과 투사체·효과·피해 노드의 구성 |
| [BP_Action_RicochetGate](Content/1_Core/Blueprints/Actions/Presets/BP_Action_RicochetGate.uasset) | Selector, 동시 실행, 투사체·효과 생성과 후속 처리 구성 |

`RootAsSubNode`는 이름만 준비된 항목이 아닙니다. `UUnitAttackComponent::ExecuteAttack_Implementation`은 기존 실행기가 있으면 공격 액션의 `RootAsSubNode`를 서브 커서로 생성하고, 그 범위에 `AttackTarget`을 저장한 뒤 실행합니다. 기본 공격을 기존 명령의 일부로 재사용하는 C++ 호출 경로입니다.

## 조합으로 달라지는 액션

같은 투사체 발사 동작도 앞뒤에 놓는 흐름과 값의 공급 방식을 바꾸면 다른 패턴이 됩니다.

| 조합 | 표현할 수 있는 차이 |
| --- | --- |
| 발사 노드 + 고정 방향 | 한 방향 발사 |
| 발사 노드 + 시간 간격을 둔 반복 | 연속 발사 |
| 발사 노드 + 반복별 각도 변경 | 부채꼴·회전형 발사 패턴 |
| 서로 다른 흐름 + 서브 커서 | 여러 방향 또는 여러 단계의 효과를 함께 진행 |
| 충돌 결과 + 후속 노드 | 맞은 위치·대상을 사용한 효과·피해 처리 |
| 같은 노드 + 다른 Claimer | 시전자, 소켓, 저장된 위치, 다른 액터를 기준으로 실행 위치 변경 |

아래는 이 조합 방식을 설명하는 **개념 예시**입니다. 특정 프리셋의 그래프를 그대로 옮긴 그림은 아닙니다.

```mermaid
flowchart TD
    I["조준 입력"] --> P["발사 준비"]
    P --> R["간격과 횟수에 따른 반복"]
    R -->|"반복마다"| B["서브 커서와 발사 방향 설정"]
    B --> S["투사체 생성"]
    S -->|"충돌 결과"| H["피해·효과 처리"]
    S -->|"액터 종료"| D["생성 액터 등록 해제"]
    R -->|"반복 완료"| E["시전 흐름 종료"]
    H --> D
```

여기서 발사 횟수·간격·각도는 패턴 구성의 문제이고, 투사체 생성과 충돌 결과 처리의 기본 역할은 재사용할 수 있습니다. 시전 흐름이 끝나는 시점과 생성된 투사체가 끝나는 시점 역시 분리하여 생각할 수 있습니다.

## 유즈맵과 GM의 활용 시나리오

다음은 현재 실행 기반을 어떤 사용자 경험으로 연결하려는지 설명하는 **설계 시나리오**입니다. 완성된 인게임 편집 화면이나 GM 기능의 사용 설명서는 아닙니다.

### 유즈맵 세팅에서 스킬을 만들어 보는 과정

유즈맵 제작자가 “짧게 준비한 뒤 세 방향으로 투사체를 발사하는 기술”을 만든다고 가정합니다.

1. 준비된 노드 종류에서 입력 선택, 준비 동작, 반복, 방향 설정, 투사체 생성을 골라 인스턴스를 만듭니다.
2. 정상 후속 연결과 반복마다 실행할 분기를 지정하고, 발사 위치·방향의 Claimer를 구성합니다.
3. Widget이나 ListView에서 반복 횟수·간격·각도를 조정하고 인디케이터로 방향을 확인합니다.
4. 테스트 유닛에 구성한 액션을 연결해 사용하고, 조작감과 실제 결과를 확인합니다.
5. 너무 조밀하면 간격을 늘리고, 다른 패턴을 원하면 분기나 후속 효과를 바꾸어 다시 실행합니다.

이 경험에서 필요한 것은 **구성·관찰·수정·재실행의 짧은 반복**입니다. 노드 조립과 값 설정을 런타임에 다룰 수 있도록 한 이유이며, 기존 프리셋은 처음부터 모든 요소를 고르지 않아도 되는 출발점으로 활용할 수 있습니다.

### 유저 GM이 밸런스와 서사를 반영하는 과정

GM의 변경은 숫자 하나를 조절하는 경우부터 새로운 행동 구조를 추가하는 경우까지 이어집니다. 다음과 같이 변화의 의미를 나누어 생각할 수 있습니다.

| GM이 의도한 변화 | 그래프에 표현할 내용 | 활용하는 기반 |
| --- | --- | --- |
| 연속 공격 사이에 대응할 여유 제공 | 준비 시간·반복 간격·횟수 조정 | 지연·반복 노드와 노드별 설정 |
| 서사적 사건으로 기존 기술에 추가 타격 부여 | 기존 흐름에 후속 효과나 추가 실행 분기 연결 | 이름 있는 연결, 서브 커서, 피해·효과 노드 |
| 보스가 특정 사건 이후 새로운 패턴 사용 | 사건을 메시지로 전달하고 새 흐름으로 전환 | 메시지 수신, `LinkedNodes`, 진입 조건 |
| 전장에 생긴 장치가 스킬의 기준점 역할 | 장치 액터의 위치·방향·문맥을 값 공급에 사용 | 액터 기반 Claimer와 월드 연결 |

장면에서 생긴 변화가 실제 플레이 규칙으로 이어지려면, 기존 노드들이 표현할 수 있는 행동과 새로 구현해야 할 행동을 구분해야 합니다. 준비된 동작은 새 연결과 설정으로 조합하고, 아직 없는 의미의 동작은 노드 클래스를 추가하여 같은 구성 체계에 포함하는 방향입니다.

### AI GM이 다룰 수 있는 실행 모델

AI GM은 상황 판단과 서사를 담당하고, 그 결과를 **노드 종류·파라미터·Claimer·연결로 구성된 구체적인 변경**으로 전달하는 계층을 둘 수 있습니다. 예를 들어 “분노한 보스가 짧은 간격의 부채꼴 공격을 얻는다”는 제안을 반복 횟수·간격·각도 규칙과 발사 분기의 구성으로 바꾸는 방식입니다.

사용자가 UI로 바꾸는 내용과 AI 측 구성 계층이 만드는 내용이 같은 객체 모델로 귀결되면, AI가 제안한 결과도 사람이 노드와 값으로 확인하고 조정할 수 있습니다. 실행은 액션 그래프의 기존 규약을 따르므로, AI 연동의 접점을 구체적인 게임 동작 단위로 정의할 수 있습니다.

현재 저장소에는 자연어를 그래프로 변환하는 AI GM 연동이 구현되어 있지 않습니다. 이를 연결하려면 노드 목록과 설정 형식을 전달하는 구성 API, 연결·값의 검증, 변경 적용 범위와 시점이 필요합니다. 이 부분은 기존 실행 모델 위에 구축할 계층입니다.

### 변경이 언제, 어디에 적용되는가

런타임 구성 UX에서는 수정할 대상과 적용 범위를 함께 표현해야 합니다. 아래 항목은 그 UX를 완성할 때 정해야 할 동작 규칙입니다.

| 변경 대상 | 정해야 할 적용 규칙 |
| --- | --- |
| 여러 곳이 참조하는 노드 인스턴스 | 공통 스킬을 함께 바꿀지, 해당 유닛·변형용 인스턴스를 별도로 구성할지 |
| 현재 진행 중인 액션 | 다음 시전부터 적용할지, 진행 중인 흐름을 종료·재시작할지, 특정 이후 단계에 반영할지 |
| 이미 생성된 투사체·타이머 | 생성 시 확정한 값을 유지할지, 이후 조회하는 설정을 변경할지 |
| 편집한 그래프 | 세션 동안 사용할지, 노드 식별자·연결·값을 저장해 다음 세션에 복원할지 |

예를 들어 `MoveExecutorToNext`는 호출 시점의 `NextNode`를 읽습니다. 따라서 객체의 연결을 바꿀 수 있다는 사실과, 진행 중인 모든 실행에 일관된 시점으로 변경이 적용된다는 것은 구분해야 합니다. 현재 참조 기반 구조 위에 적용 정책을 정하면, 사용자가 수정 결과의 범위를 이해하며 실험할 수 있는 UX로 이어갈 수 있습니다.

## 새 액션과 노드 추가하기

### 기존 노드를 조립하는 경우

아래는 현재 Blueprint 프리셋을 구성하며 확인할 순서입니다. 인게임 편집 UX에서도 같은 생성·설정·연결 작업을 사용자 조작에 대응시키는 것을 목표로 합니다.

1. [`BP_ActionBase`](Content/1_Core/Blueprints/Actions/BP_ActionBase.uasset)와 가까운 기존 프리셋을 참고해 액션 Blueprint를 구성합니다.
2. 필요한 노드 객체를 만들고 `RootNode`, `NextNode`, `CanceledNode`, 이름 있는 연결을 설정합니다.
3. 입력이 필요하면 Selector에 입력 종류·태그·입력 수락 후 연결을 지정합니다. 노드가 읽는 태그와 기록하는 태그의 타입도 맞춥니다.
4. 위치·방향·대상·수치를 Claimer로 공급하고, 서브 커서에서 바뀌어야 하는 값의 범위를 정합니다.
5. Selector의 `IndicatorNodes`와 Behavior 노드의 미리 보기를 연결합니다.
6. [`BP_ActionSetting`](Content/1_Core/Blueprints/Settings/BP_ActionSetting.uasset), [`DT_ActionPresets`](Content/1_Core/DataTables/DT_ActionPresets.uasset), 해당 유닛의 `ActionList`를 확인하여 액션 조회와 사용 가능 목록에 연결합니다.
7. 테스트 맵에서 정상 완료뿐 아니라 입력 취소, 행동 중단, 생성 액터 종료 후에도 흐름이 정리되는지 확인합니다.

### 새로운 동작이 필요한 경우

`UActionNode` 또는 목적에 맞는 `UActionBehaviorNode`, `UActionDelayNode`, `UActionSpawnNode` 계열을 확장합니다. 핵심 규약은 다음과 같습니다.

- `ClaimExecute` / `ClaimExecuteWithInput`에서 시작하고, 완료 조건을 만족했을 때 전환 API를 호출합니다.
- 실행마다 달라지는 데이터는 커서의 값 범위 또는 커서를 키로 한 상태에 보관합니다.
- 정상 완료와 취소 시 정리할 내용을 각각 정의합니다. 비동기 콜백이 남는 경우 해당 실행과 현재 노드가 여전히 유효한지 확인합니다.
- 메인 행동을 차지하는지, 기존 행동을 중단할 수 있는지, 별도 서브 커서로 실행할지를 결정합니다.
- 액터를 생성하면 등록과 종료 통지를 연결하고, 미리 보기가 필요하면 인디케이터 요청·갱신도 구현합니다.

노드가 반환되었다고 그래프가 자동 진행하는 구조가 아니므로, **완료 시 전환을 빠뜨리면 커서가 남고, 중복 전환을 호출하면 의도하지 않은 후속 실행이 생길 수 있습니다.** 노드 작성 시 가장 먼저 확인할 계약입니다.

## 확장 가능성

다음은 현재 구조를 활용해 확장할 수 있는 방향입니다. 모두 완성된 게임 기능을 뜻하지 않으며, 현재 제공하는 접점과 추가 작업을 구분했습니다.

| 확장 방향 | 활용할 수 있는 기반 | 추가로 구성·구현할 부분 |
| --- | --- | --- |
| 인게임 스킬 실험 UX | UObject 생성·참조 연결, Claimer, 입력 인디케이터, 실행 진입점 | 노드 선택·속성 편집·연결 UI, 테스트 실행, 결과 표시와 변경 이력 |
| 유저 GM의 규칙 개입 | 노드 설정과 연결 변경, 메시지·서브 커서 | 변경 대상·범위·시점의 조작, 진행 중인 액션에 대한 적용 정책 |
| AI GM의 규칙 구성 | 사람의 편집과 공유할 수 있는 노드·값·연결 모델 | 상황 판단과 구성 API의 연결, 구조·값 검증, 적용 결과 확인 |
| 월드에서 조작하는 노드 | UObject 참조, Actor 문맥, 생성 액터·충돌 결과와 실행의 접점 | 노드 표현 액터, 물리·상호작용의 의미 매핑, 공간상의 연결 UX |
| 유즈맵 구성 저장·복원 | 객체로 구분되는 노드와 명시적인 연결 | 저장 형식, 노드 식별자, 연결 복원, 버전 관리 |
| 스킬 변형·강화 | 재사용 가능한 발사·반복·각도 변경 노드와 Claimer | 강화 데이터가 노드 설정·그래프 구성을 바꾸는 규칙 |
| 차지·채널링 | 커서를 유지하는 노드, 시간·입력·메시지 처리 | 유지·해제 조건, 비용, 단계별 효과, 취소 정리 |
| 보스의 연속 패턴 | 지연·반복·서브 커서, 위치·대상 문맥 | AI 진입부, 패턴 선택과 페이즈 전환 |
| 명중 후 연쇄 효과 | 충돌 결과 값, 생성 액터 문맥, 후속 노드 연결 | 다음 대상 탐색, 연쇄 제한, 효과별 종료 조건 |
| 유닛별 공격 재사용 | `RootAsSubNode`, `AttackTarget`, 유닛 액션 태그 | 유닛별 공격 프리셋과 사거리·모션·피해 규칙 |

표현력을 늘리는 방법은 두 가지입니다. 기존 동작을 다른 연결·시간·값 규칙으로 조립하거나, 새로운 의미의 노드를 하나 추가해 기존 노드들과 함께 사용합니다. 설계의 관찰 지점은 **이 실행 기반이 유즈맵 제작자·플레이어·GM이 게임 안에서 규칙을 구성하는 경험으로 어떻게 이어지는가**입니다.

## 실행 환경

| 항목 | 저장소 설정 |
| --- | --- |
| 엔진 | Unreal Engine **5.6** — [WhiteCladWarriors.uproject](WhiteCladWarriors.uproject) |
| 프로젝트 모듈 | `WhiteCladWarriors` Runtime — [Build.cs](Source/WhiteCladWarriors/WhiteCladWarriors.Build.cs) |
| 에디터 타깃 | `WhiteCladWarriorsEditor` — [Target.cs](Source/WhiteCladWarriorsEditor.Target.cs) |
| 기본 맵 | `/Game/1_Core/Levels/Test` — [DefaultEngine.ini](Config/DefaultEngine.ini) |
| 기본 게임 모드 | `BP_GameMode_BossRunning` — [Blueprint](Content/1_Core/Blueprints/BP_GameMode_BossRunning.uasset) |

1. 저장소를 내려받고 Unreal Engine 5.6 및 해당 엔진용 C++ 빌드 도구를 준비합니다.
2. `WhiteCladWarriors.uproject`의 프로젝트 파일을 생성한 뒤 `WhiteCladWarriorsEditor` 타깃을 빌드합니다. Windows에서는 `Development Editor` 구성을 사용할 수 있습니다.
3. 프로젝트를 열고 `Content/1_Core/Levels/Test` 맵과 위의 액션 프리셋을 확인합니다.
4. Editor에서 Play로 동작을 확인합니다. 실행에는 C++ 소스뿐 아니라 참조된 `Content`, `Config`, `Plugins`도 필요합니다.

저장소에는 게임 리소스가 함께 들어 있으므로 전체 내려받기에 시간이 걸릴 수 있습니다. 구조만 읽으려면 우선 `Source/WhiteCladWarriors`와 `Content/1_Core/Blueprints/Actions`부터 탐색하면 됩니다. [KawaiiPhysics](Plugins/KawaiiPhysics)는 함께 포함된 외부 플러그인으로, 위에서 설명한 액션 그래프 코드와 구분됩니다.

## 개발 상태와 읽을 때의 주의점

- **프로토타입 단계입니다.** 현재 노드·커서·실행기·값 범위와 Blueprint 콘텐츠 조립 구조가 중심입니다. 인게임 편집기, 노드의 Widget·ListView 편집 연동, 월드에서 조작하는 노드, 유저 GM·AI GM 기능은 이 문서에 설명한 설계 목표입니다.
- **구체적인 동작 일부는 Blueprint에 있습니다.** `BlueprintImplementableEvent`가 C++에서 비어 있다는 사실만으로 해당 기능의 부재를 판단할 수 없습니다. 반대로 에셋 이름이나 참조만으로 모든 실행 경로가 검증됐다고 판단해서도 안 됩니다.
- **미완성 API도 있습니다.** `EInputType::MultiTarget`은 정의되어 있으나 현재 C++ Selector의 해당 처리 경로는 주석 상태이고, `UActorArrayClaimer::GetActorArray`는 빈 배열을 반환합니다. 단일 대상 입력 및 여러 컴포넌트를 담는 실행 구조와 별도로 확인해야 합니다.
- **네트워크 동기화와 런타임 검증은 별도 과제입니다.** 컴포넌트별 커서나 논리적 동시 실행이 곧 멀티플레이 복제·결정론·성능 검증을 의미하지는 않습니다. 이 문서는 소스와 에셋 구성을 기준으로 작성했으며, 빌드·PIE 실행 결과나 성능 측정치는 포함하지 않습니다.

액션 그래프 외의 코드는 [유닛 컴포넌트](Source/WhiteCladWarriors/Public/Objects/Selectables/Components), [UI 기반](Source/WhiteCladWarriors/Public/Objects/Generals/Widgets), [인벤토리](Source/WhiteCladWarriors/Public/Items), [코어 Blueprint](Content/1_Core/Blueprints)에서 이어서 살펴볼 수 있습니다.
