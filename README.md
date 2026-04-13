# 언리얼 엔진 애니메이션 시스템 구현 과정

## 개요
언리얼 엔진 5의 멀티스레드 기반 애니메이션 시스템 구현 과정을 정리한 문서입니다.
라이라(Lyra) 프로젝트를 참고하여 제작하였습니다.

---

## 시스템 구조

```
게임 스레드                    워커 스레드
─────────────────────────────────────────
Event Graph                   Thread Safe Update Animation
    ↓                               ↓
게임 로직 처리              Property Access로 데이터 복사
    ↓                               ↓
캐릭터 이동/입력 등          애니메이션 변수 업데이트
                                    ↓
                              AnimGraph에서 포즈 계산
                                    ↓
                              Final Animation Pose 출력
```

---

## 구현 과정

### 1단계 - 프로젝트 설정

- Project Settings → Engine → General Settings → Anim Blueprints
    - `Allow Multi Threaded Animation Update` 활성화

### 2단계 - 애니메이션 블루프린트 설정

- 애니메이션 블루프린트 열기
- Class Settings → `Use Multi Threaded Animation Update` 활성화
- Event Graph 비우기

### 3단계 - Property Access 설정

- Thread Safe Update Animation 함수 내부에서 우클릭
- `Property Access` 검색 후 노드 생성
- 접근할 데이터 경로 바인드
    - 예) `GetMovementComponent.GetCurrentAcceleration`

### 4단계 - Thread Safe Update Animation 구현

- Property Access로 데이터 수집
    - 캐릭터 속도 (Velocity)
    - 이동 방향 (LocalVelocityDirection)
    - 가속도 (Acceleration)
    - 공중 여부 (IsFalling)
- 수집한 데이터 가공
    - CardinalDirection 결정
    - 상태 판단 (이동/정지/공중)


---

## 전체 흐름

```
입력 발생
    ↓
[게임 스레드]
CharacterMovement 업데이트
    ↓
Property Access 데이터 복사
    ↓
[워커 스레드]
BlueprintThreadSafeUpdateAnimation 실행
    ↓
LocalVelocityDirection 계산
LocalVelocityDirectionNoOffset 계산
CardinalDirection 결정
    ↓
AnimGraph State Machine 업데이트
    ↓
최종 포즈 적용
```

---



## 참고 자료

- [언리얼 엔진 공식 Animation Optimization 문서](https://dev.epicgames.com/documentation/en-us/unreal-engine/animation-optimization-in-unreal-engine)
- 라이라(Lyra) 스타터 게임 프로젝트

