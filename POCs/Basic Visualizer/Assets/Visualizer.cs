using System;
using System.Collections;
using System.Collections.Generic;
using System.Numerics;
using UnityEngine;
using Vector2 = UnityEngine.Vector2;
using Vector3 = UnityEngine.Vector3;
using Vector4 = System.Numerics.Vector4;

public class NewBehaviourScript : MonoBehaviour
{
    private Queue<GameObject> _quadPool = new Queue<GameObject>();
    private int _poolSize = 5000;

    // Start is called before the first frame update
    void Start()
    {
        for (int i = 0; i < _poolSize; i++)
        {
            GameObject quadObject = new GameObject("QuadBillboardObject");
            QuadBillboard quadBillboard = quadObject.AddComponent<QuadBillboard>();

            quadObject.SetActive(false);

            _quadPool.Enqueue(quadObject);
        }
    }

    // Update is called once per frame
    void Update()
    {
        int width = 320, height = 180;
        float aspectRatio = width / (float)height;
        float length = 2.0f;
        float radius = 0.5f;
        Vector3 sphereOrigin = Vector3.zero;

        Vector3 lightDir = new Vector3(-1, -1, 1);
        lightDir.Normalize();

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                Vector2 coord = new Vector2((float)x / width, (float)y / height);
                coord = coord * 2.0f - Vector2.one; // -1 -> 1
                coord.x *= aspectRatio;

                Vector3 rayOrigin = new Vector3(0.0f, 0.0f, 2.0f);
                Vector3 rayDirection = new Vector3(coord.x, coord.y, -1.0f);


                float a = UnityEngine.Vector3.Dot(rayDirection, rayDirection);
                float b = 2.0f * UnityEngine.Vector3.Dot(rayOrigin, rayDirection);
                float c = UnityEngine.Vector3.Dot(rayOrigin, rayOrigin) - (radius * radius);

                Color rayColor = new Color(0, 0, 0, 0.001f);
                float discriminant = (b * b) - 4.0f * a * c;

                if (discriminant >= 0.0f)
                {
                    float[] t = new float[]
                    {
                        (-b - MathF.Sqrt(discriminant)) / (2.0f * a),
                        (-b + MathF.Sqrt(discriminant)) / (2.0f * a)
                    };
                    for (int i = 0; i < 2; i++)
                    {
                        Vector3 hitPosition = rayOrigin + rayDirection * t[i];
                        Vector3 normal = hitPosition - sphereOrigin;
                        normal.Normalize();

                        float light = MathF.Max(Vector3.Dot(normal, -lightDir), 0.0f);

                        UseQuadFromPool(hitPosition, 0.03f, new Color(
                            light * Color.green.r,
                            light * Color.green.g,
                            light * Color.green.b, 
                            1.0f));
                    }

                    rayColor = new Color(1,0,1,0.01f);
                }

                Debug.DrawLine(rayOrigin, rayOrigin + rayDirection * length, rayColor);
            }
        }
    }

    private void UseQuadFromPool(Vector3 position, float size, Color color)
    {
        if (_quadPool.Count == 0)
        {
            GameObject oldestQuad = _quadPool.Dequeue();
            oldestQuad.SetActive(false);
            _quadPool.Enqueue(oldestQuad);
        }

        GameObject quadObject = _quadPool.Dequeue();
        quadObject.SetActive(true); // Activate the quad

        QuadBillboard quadBillboard = quadObject.GetComponent<QuadBillboard>();

        quadBillboard.DrawQuad(position, size, color);

        _quadPool.Enqueue(quadObject);
    }
}
