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

# 功能

## 抗锯齿

超采样即可。

## 软阴影

自带效果。

## Color Bleeding

自带效果。

## 景深效果

### 相机参数

在 Camera 的基础上加入：

* scale(整形，表示实际像平面大小到图像大小的比例尺度，越大则视角越小)
* focalDist(焦距f)
* imageDist(成像平面到透镜的距离p')
* aperture(光圈直径，即在透镜上随机选点时的圆半径)

得到 LensCamera。

### 效果实现

为了实现相机效果，像平面应在一到二倍焦距之间，对焦的物体将在二倍焦距之外。

成像平面即最终成图的地方，令像距为 p'，对应物距为 p (此距离能清晰成像)，焦距为 f，利用相似可以得到 
$$
\frac{1}{f} = \frac{1}{p} + \frac{1}{p'}
$$
求出 p 之后，便可以求出像平面上一点对应的对焦物体对应的点，将透镜上随机得到的点同该点连线即可得到射线。

# DEBUG

* 对于光源，其 color 应当设置为 (0, 0, 0)，否则容易出现在光源处来回弹导致画面全白
