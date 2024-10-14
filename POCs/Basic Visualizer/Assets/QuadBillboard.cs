using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(MeshFilter), typeof(MeshRenderer))]
public class QuadBillboard : MonoBehaviour
{
    private MeshFilter _meshFilter;
    private MeshRenderer _meshRenderer;

    public void DrawQuad(Vector3 position, float size, Color color)
    {
        if (_meshFilter == null || _meshRenderer == null)
        {
            InitializeComponents();
        }

        Camera camera = Camera.main;
        if (camera == null) return;

        _meshRenderer.material.color = color;

        Mesh mesh = new Mesh();

        Vector3 up = Vector3.up * size / 2;
        Vector3 right = Vector3.right * size / 2;

        Vector3[] vertices = new Vector3[4];
        vertices[0] = position - right + up;  // Top-left
        vertices[1] = position + right + up;  // Top-right
        vertices[2] = position + right - up;  // Bottom-right
        vertices[3] = position - right - up;  // Bottom-left

        int[] triangles = new int[6]
        {
            0, 1, 2, // First triangle
            0, 2, 3  // Second triangle
        };

        mesh.vertices = vertices;
        mesh.triangles = triangles;
        mesh.RecalculateNormals();

        _meshFilter.mesh = mesh;
    }

    private void InitializeComponents()
    {
        _meshFilter = GetComponent<MeshFilter>();
        _meshRenderer = GetComponent<MeshRenderer>();

        if (_meshRenderer == null)
        {
            _meshRenderer = gameObject.AddComponent<MeshRenderer>();
        }

        if (_meshFilter == null)
        {
            _meshFilter = gameObject.AddComponent<MeshFilter>();
        }

        if (_meshRenderer.material == null)
        {
            _meshRenderer.material = new Material(Shader.Find("Unlit/Color"));
        }
    }

    // Start is called before the first frame update
    void Start()
    {
        _meshFilter = GetComponent<MeshFilter>();
        _meshRenderer = GetComponent<MeshRenderer>();

        _meshRenderer.material = new Material(Shader.Find("Unlit/Color"));
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
