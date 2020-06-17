## TODO

* 修改求交算法使得能在圆内求交。

## scene\_parser 相关修改

* 不再区分光源和普通物体，删去 Lights 相关处理。
* 材质相关配置中，参数有 反射占比 refRatio，发光 emission，颜色 color，例如：

```
Materials {
    numMaterials 2
    Material {
        refRatio 1
        emission 12 12 12
        color 0 0 0
    }
    Material {
        refRatio 0
        emission 0 0 0
        color 1 1 1
    }
}
```

## DEBUG

* 对于光源，其 color 应当设置为 (0, 0, 0)，否则容易出现在光源处来回弹导致画面全白
