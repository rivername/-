using UnityEngine;

public class AndorExplosion : MonoBehaviour
{
    void Update()
    {
        var animator = GetComponent<Animator>();
        var animinfo = animator.GetCurrentAnimatorStateInfo(0);
        var frame = animinfo.normalizedTime;

        if (frame > 1.0f)
        {
            Destroy(gameObject);
        }
    }
}
